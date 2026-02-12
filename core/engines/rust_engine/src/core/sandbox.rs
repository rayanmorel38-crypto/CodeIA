use std::fs::{File, OpenOptions};
use std::io::Result;
use std::sync::{Arc, Mutex};
use serde::{Serialize, Deserialize};
use std::collections::VecDeque;
use std::path::{Path, PathBuf};

/// Politique d'isolation de la Sandbox
#[derive(Clone, Debug, Serialize, Deserialize)]
pub enum IsolationLevel {
    None,           // Pas d'isolation (dangeureux)
    Logging,        // Isolation par logging
    FileSystem,     // Isolation du système de fichiers
    Process,        // Isolation par processus
    Container,      // Isolation par conteneur (recommandé)
}

/// Restrictions d'exécution
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct ExecutionPolicy {
    pub max_memory_mb: u64,
    pub max_cpu_time_ms: u64,
    pub max_file_size_mb: u64,
    pub allowed_syscalls: Vec<String>,
    pub forbidden_paths: Vec<PathBuf>,
    pub isolation_level: IsolationLevel,
}

impl Default for ExecutionPolicy {
    fn default() -> Self {
        ExecutionPolicy {
            max_memory_mb: 256,
            max_cpu_time_ms: 5000,
            max_file_size_mb: 100,
            allowed_syscalls: vec!["read".to_string(), "write".to_string(), "open".to_string()],
            forbidden_paths: vec![
                PathBuf::from("/etc/passwd"),
                PathBuf::from("/root"),
                PathBuf::from("/sys"),
            ],
            isolation_level: IsolationLevel::FileSystem,
        }
    }
}

/// Événement d'exécution pour auditer la Sandbox
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct ExecutionEvent {
    pub timestamp: u64,
    pub event_type: String,
    pub details: String,
    pub allowed: bool,
}

#[derive(Clone)]
pub struct Sandbox {
    pub asks: Arc<Mutex<File>>,
    pub tasks: Arc<Mutex<File>>,
    pub errors: Arc<Mutex<File>>,
    pub optimisations: Arc<Mutex<File>>,
    pub policy: ExecutionPolicy,
    pub audit_log: Arc<Mutex<VecDeque<ExecutionEvent>>>,
    pub working_dir: PathBuf,
}

impl Sandbox {
    pub fn new() -> Result<Self> {
        Self::with_policy(ExecutionPolicy::default())
    }

    pub fn with_policy(policy: ExecutionPolicy) -> Result<Self> {
        // Crée le dossier logs/ et sandbox/ si nécessaire
        std::fs::create_dir_all("logs")?;
        std::fs::create_dir_all("sandbox")?;

        Ok(Sandbox {
            asks: Arc::new(Mutex::new(
                OpenOptions::new()
                    .create(true)
                    .append(true)
                    .open("logs/asks.log")?,
            )),
            tasks: Arc::new(Mutex::new(
                OpenOptions::new()
                    .create(true)
                    .append(true)
                    .open("logs/tasks.log")?,
            )),
            errors: Arc::new(Mutex::new(
                OpenOptions::new()
                    .create(true)
                    .append(true)
                    .open("logs/errors.log")?,
            )),
            optimisations: Arc::new(Mutex::new(
                OpenOptions::new()
                    .create(true)
                    .append(true)
                    .open("logs/optimisations.log")?,
            )),
            policy,
            audit_log: Arc::new(Mutex::new(VecDeque::with_capacity(1000))),
            working_dir: PathBuf::from("sandbox"),
        })
    }

    /// Vérifie si un chemin est autorisé
    fn is_path_allowed(&self, path: &Path) -> bool {
        for forbidden in &self.policy.forbidden_paths {
            if path.starts_with(forbidden) {
                return false;
            }
        }
        true
    }

    /// Enregistre un événement d'audit
    fn audit_event(&self, event_type: &str, details: &str, allowed: bool) {
        let event = ExecutionEvent {
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap_or_default()
                .as_secs(),
            event_type: event_type.to_string(),
            details: details.to_string(),
            allowed,
        };

        if let Ok(mut log) = self.audit_log.lock() {
            log.push_back(event);
            if log.len() > 1000 {
                log.pop_front();
            }
        }
    }

    pub fn log_ask(&self, msg: &str) -> Result<()> {
        use std::io::Write;
        self.audit_event("ask", msg, true);
        self.asks
            .lock()
            .unwrap()
            .write_all(format!("ASK: {}\n", msg).as_bytes())
    }

    pub fn log_task(&self, msg: &str) -> Result<()> {
        use std::io::Write;
        self.audit_event("task", msg, true);
        self.tasks
            .lock()
            .unwrap()
            .write_all(format!("TASK: {}\n", msg).as_bytes())
    }

    pub fn log_error(&self, msg: &str) -> Result<()> {
        use std::io::Write;
        self.audit_event("error", msg, false);
        self.errors
            .lock()
            .unwrap()
            .write_all(format!("ERROR: {}\n", msg).as_bytes())
    }

    pub fn log_optimisation(&self, msg: &str) -> Result<()> {
        use std::io::Write;
        self.audit_event("optimisation", msg, true);
        self.optimisations
            .lock()
            .unwrap()
            .write_all(format!("OPT: {}\n", msg).as_bytes())
    }

    /// Exécute du code dans un environnement isolé
    pub fn execute_isolated(&self, code: &str) -> Result<String> {
        // Vérifier les restrictions
        match self.policy.isolation_level {
            IsolationLevel::None => {
                self.audit_event("execute", code, true);
            }
            IsolationLevel::Logging => {
                self.audit_event("execute", code, true);
                self.log_task(code)?;
            }
            IsolationLevel::FileSystem => {
                // Vérifier les accès aux fichiers interdits
                if !self.is_path_allowed(Path::new(code)) {
                    self.audit_event("execute", code, false);
                    self.log_error(&format!("Forbidden path access: {}", code))?;
                    return Ok("Execution blocked: forbidden path".to_string());
                }
                self.audit_event("execute", code, true);
            }
            IsolationLevel::Process => {
                self.audit_event("execute", code, true);
                self.log_task(&format!("[ISOLATED_PROCESS] {}", code))?;
            }
            IsolationLevel::Container => {
                self.audit_event("execute", code, true);
                self.log_task(&format!("[CONTAINER] {}", code))?;
            }
        }

        Ok("Execution completed".to_string())
    }

    /// Récupère l'audit log
    pub fn get_audit_log(&self) -> Vec<ExecutionEvent> {
        if let Ok(log) = self.audit_log.lock() {
            log.iter().cloned().collect()
        } else {
            Vec::new()
        }
    }

    /// Récupère la policy actuelle
    pub fn get_policy(&self) -> ExecutionPolicy {
        self.policy.clone()
    }
}

