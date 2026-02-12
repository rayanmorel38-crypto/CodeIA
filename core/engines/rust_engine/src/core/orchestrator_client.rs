// src/orchestrator_client.rs
use crate::utils::error::{EngineError, Result};
use serde::{Deserialize, Serialize};
use std::time::Duration;

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct OrchestratorRequest {
    pub task_type: String,
    pub data: serde_json::Value,
    pub timeout_seconds: Option<u32>,
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct Task {
    pub id: String,
    pub name: String,
    pub task_type: String,
    pub data: serde_json::Value,
    pub priority: u8,
}

#[derive(Debug, Serialize, Deserialize, Clone)]
pub struct OrchestratorResponse {
    pub success: bool,
    pub result: String,
    pub duration_ms: u128,
}

pub struct OrchestratorClient {
    base_url: String,
    client: reqwest::Client,
}

impl OrchestratorClient {
    pub fn new(host: &str, port: u16) -> Self {
        let base_url = format!("http://{}:{}", host, port);
        let client = reqwest::Client::new();
        
        OrchestratorClient { base_url, client }
    }

    /// Send a task request to orchestrator
    pub async fn analyze(&self, code: &str, language: &str) -> Result<OrchestratorResponse> {
        let mut data = serde_json::Map::new();
        data.insert("code".to_string(), serde_json::json!(code));
        data.insert("language".to_string(), serde_json::json!(language));

        let request = OrchestratorRequest {
            task_type: "analyze".to_string(),
            data: serde_json::Value::Object(data),
            timeout_seconds: Some(30),
        };

        self.send_request(&request).await
    }

    /// Request code optimization from orchestrator
    pub async fn optimize(&self, code: &str) -> Result<OrchestratorResponse> {
        let mut data = serde_json::Map::new();
        data.insert("code".to_string(), serde_json::json!(code));

        let request = OrchestratorRequest {
            task_type: "optimize".to_string(),
            data: serde_json::Value::Object(data),
            timeout_seconds: Some(30),
        };

        self.send_request(&request).await
    }

    /// Health check
    pub async fn health_check(&self) -> Result<bool> {
        let url = format!("{}/health", self.base_url);
        
        match self.client
            .get(&url)
            .timeout(Duration::from_secs(5))
            .send()
            .await {
            Ok(resp) => Ok(resp.status().is_success()),
            Err(e) => Err(EngineError::IntegrationError(format!(
                "Health check failed: {}",
                e
            ))),
        }
    }

    /// Poll for next task from orchestrator
    pub async fn poll_task(&self) -> Result<Task> {
        let url = format!("{}/poll", self.base_url);
        
        match self.client
            .get(&url)
            .timeout(Duration::from_secs(30))
            .send()
            .await {
            Ok(resp) => {
                if resp.status().is_success() {
                    match resp.json::<Task>().await {
                        Ok(task) => Ok(task),
                        Err(e) => Err(EngineError::IntegrationError(format!(
                            "Failed to parse task: {}",
                            e
                        ))),
                    }
                } else if resp.status() == reqwest::StatusCode::NO_CONTENT {
                    // No task available
                    Err(EngineError::IntegrationError("No task available".to_string()))
                } else {
                    Err(EngineError::IntegrationError(format!(
                        "Poll failed with status: {}",
                        resp.status()
                    )))
                }
            }
            Err(e) => Err(EngineError::IntegrationError(format!(
                "Poll request failed: {}",
                e
            ))),
        }
    }

    /// Generic send request
    async fn send_request(&self, request: &OrchestratorRequest) -> Result<OrchestratorResponse> {
        let url = format!("{}/{}", self.base_url, request.task_type);
        let start = std::time::Instant::now();

        match self.client
            .post(&url)
            .json(request)
            .timeout(Duration::from_secs(60))
            .send()
            .await {
            Ok(resp) => {
                let duration_ms = start.elapsed().as_millis();
                match resp.json::<OrchestratorResponse>().await {
                    Ok(mut result) => {
                        result.duration_ms = duration_ms;
                        Ok(result)
                    }
                    Err(e) => Err(EngineError::IntegrationError(format!(
                        "Failed to parse orchestrator response: {}",
                        e
                    ))),
                }
            }
            Err(e) => Err(EngineError::IntegrationError(format!(
                "Orchestrator request failed: {}",
                e
            ))),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    #[ignore] // Skip unless orchestrator running
    async fn test_orchestrator_connection() {
        let client = OrchestratorClient::new("127.0.0.1", 8000);
        let result = client.health_check().await;
        assert!(result.is_ok());
    }
}
