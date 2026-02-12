// src/main.rs - Worker or HTTP server process managed by orchestrator
use rust_engine::core::ai_core::{AICore, Role};
use rust_engine::core::orchestrator_client::Task;
use rust_engine::core::sandbox::Sandbox;
use rust_engine::utils::debug_writer::DebugWriter;
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use std::convert::Infallible;
use std::env;
use std::sync::Arc;
use std::time::{SystemTime, UNIX_EPOCH};
use tokio::sync::Mutex;
use warp::http::StatusCode;
use warp::Filter;

#[derive(Clone)]
struct AppState {
    ai_core: Arc<Mutex<AICore>>,
    sandbox: Arc<Sandbox>,
    orchestrator_token: String,
}

#[derive(Debug, Serialize)]
struct HealthResponse {
    status: String,
    ready: bool,
    mode: String,
}

#[derive(Debug, Serialize)]
struct CapabilitiesResponse {
    status: String,
    engine: String,
    capabilities: Vec<&'static str>,
}

#[derive(Debug, Serialize)]
struct ProcessResponse {
    status: String,
    result: String,
    task_id: String,
    task_type: String,
}

#[derive(Debug, Deserialize)]
struct ProcessRequest {
    #[serde(default)]
    id: Option<String>,
    #[serde(default)]
    task_id: Option<String>,
    #[serde(default)]
    name: Option<String>,
    #[serde(default)]
    task_type: Option<String>,
    #[serde(default)]
    operation: Option<String>,
    #[serde(default)]
    data: Option<Value>,
    #[serde(default)]
    priority: Option<u8>,
}

#[tokio::main]
async fn main() {
    rust_engine::utils::tracing_config::init_tracing();
    rust_engine::core::metrics::register_metrics();

    // Parse args: --server --port=3002 / --port 3002
    let args: Vec<String> = env::args().collect();
    let server_mode = args.iter().any(|a| a == "--server");
    let port = parse_port(&args).unwrap_or_else(|| {
        env::var("PORT")
            .ok()
            .and_then(|s| s.parse::<u16>().ok())
            .unwrap_or(3002)
    });

    // Optional hard-gating: if launched by orchestrator env, require token in requests.
    let orchestrator_token = env::var("CODEIA_ORCHESTRATOR_TOKEN")
        .or_else(|_| env::var("ORCHESTRATOR_TOKEN"))
        .or_else(|_| env::var("RUST_ENGINE_TOKEN"))
        .unwrap_or_default();

    let ai_core = Arc::new(Mutex::new(AICore::new()));
    let sandbox = match Sandbox::new() {
        Ok(s) => Arc::new(s),
        Err(e) => {
            DebugWriter::error(&format!("Failed to initialize sandbox: {}", e));
            return;
        }
    };

    DebugWriter::info("═══════════════════════════════════════════════════════");
    DebugWriter::info("  RUST ENGINE - Orchestrator Compatible Runtime");
    DebugWriter::info("═══════════════════════════════════════════════════════");
    DebugWriter::info("✓ AICore initialized");

    if server_mode {
        let metrics_port = env::var("METRICS_PORT")
            .ok()
            .and_then(|s| s.parse::<u16>().ok())
            .unwrap_or(port.saturating_add(5000));

        tokio::spawn(async move {
            env::set_var("METRICS_PORT", metrics_port.to_string());
            rust_engine::core::metrics::serve_metrics().await;
        });

        DebugWriter::info(&format!("✓ HTTP server mode enabled on 127.0.0.1:{}", port));
        DebugWriter::info(&format!("✓ Metrics server running on 127.0.0.1:{}", metrics_port));

        let state = AppState {
            ai_core,
            sandbox,
            orchestrator_token,
        };

        run_http_server(port, state).await;
        return;
    }

    DebugWriter::info("✓ Worker mode enabled");
    DebugWriter::info("AWAITING ORCHESTRATOR COMMANDS...");

    loop {
        tokio::time::sleep(tokio::time::Duration::from_secs(60)).await;
    }
}

fn parse_port(args: &[String]) -> Option<u16> {
    for (idx, arg) in args.iter().enumerate() {
        if let Some(v) = arg.strip_prefix("--port=") {
            if let Ok(p) = v.parse::<u16>() {
                return Some(p);
            }
        }
        if arg == "--port" {
            if let Some(next) = args.get(idx + 1) {
                if let Ok(p) = next.parse::<u16>() {
                    return Some(p);
                }
            }
        }
    }
    None
}

async fn run_http_server(port: u16, state: AppState) {
    let state_filter = warp::any().map(move || state.clone());

    let health = warp::path("health")
        .and(warp::get())
        .map(|| {
            let payload = HealthResponse {
                status: "ok".to_string(),
                ready: true,
                mode: "orchestrator".to_string(),
            };
            warp::reply::json(&payload)
        });

    let capabilities = warp::path("capabilities")
        .and(warp::get())
        .map(|| {
            let payload = CapabilitiesResponse {
                status: "success".to_string(),
                engine: "rust".to_string(),
                capabilities: vec!["async_execute", "performance", "parallel", "analyze", "optimize", "simulate", "plan"],
            };
            warp::reply::json(&payload)
        });

    let process = warp::path("process")
        .and(warp::post())
        .and(warp::header::optional::<String>("x-orchestrator-token"))
        .and(warp::header::optional::<String>("authorization"))
        .and(warp::header::optional::<String>("x-api-key"))
        .and(warp::header::optional::<String>("x-auth-token"))
        .and(warp::body::json::<ProcessRequest>())
        .and(state_filter.clone())
        .and_then(handle_process);

    let routes = health.or(capabilities).or(process);
    warp::serve(routes).run(([127, 0, 0, 1], port)).await;
}

async fn handle_process(
    x_token: Option<String>,
    authorization: Option<String>,
    x_api_key: Option<String>,
    x_auth_token: Option<String>,
    req: ProcessRequest,
    state: AppState,
) -> Result<impl warp::Reply, Infallible> {
    if !is_authorized(
        &state.orchestrator_token,
        x_token.as_deref(),
        authorization.as_deref(),
        x_api_key.as_deref(),
        x_auth_token.as_deref(),
    ) {
        let body = json!({"status":"error","message":"unauthorized"});
        return Ok(warp::reply::with_status(
            warp::reply::json(&body),
            StatusCode::UNAUTHORIZED,
        ));
    }

    let task = normalize_task(req);
    let roles = vec![Role::Admin];

    let result = {
        let core = state.ai_core.lock().await;
        core.execute_task(&task, &roles, &state.sandbox).await
    };

    match result {
        Ok(text) => {
            let body = ProcessResponse {
                status: "success".to_string(),
                result: text,
                task_id: task.id,
                task_type: task.task_type,
            };
            Ok(warp::reply::with_status(
                warp::reply::json(&body),
                StatusCode::OK,
            ))
        }
        Err(e) => {
            let body = json!({
                "status": "error",
                "message": e.to_string(),
                "task_id": task.id,
                "task_type": task.task_type
            });
            Ok(warp::reply::with_status(
                warp::reply::json(&body),
                StatusCode::BAD_REQUEST,
            ))
        }
    }
}

fn normalize_task(req: ProcessRequest) -> Task {
    let now_ms = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_millis())
        .unwrap_or(0);

    let task_type = req
        .task_type
        .or(req.operation)
        .unwrap_or_else(|| "analyze".to_string());

    let data = req.data.unwrap_or_else(|| json!({}));

    Task {
        id: req.task_id.or(req.id).unwrap_or_else(|| format!("rust-task-{}", now_ms)),
        name: req.name.unwrap_or_else(|| task_type.clone()),
        task_type,
        data,
        priority: req.priority.unwrap_or(1),
    }
}

fn is_authorized(
    expected_token: &str,
    x_token: Option<&str>,
    authorization: Option<&str>,
    x_api_key: Option<&str>,
    x_auth_token: Option<&str>,
) -> bool {
    // No token configured => permissive mode (for local tests)
    if expected_token.is_empty() {
        return true;
    }

    if let Some(t) = x_token {
        if t == expected_token {
            return true;
        }
    }
    if let Some(t) = x_api_key {
        if t == expected_token {
            return true;
        }
    }
    if let Some(t) = x_auth_token {
        if t == expected_token {
            return true;
        }
    }
    if let Some(auth) = authorization {
        let bearer = auth.trim();
        if bearer == format!("Bearer {}", expected_token) {
            return true;
        }
    }

    false
}

