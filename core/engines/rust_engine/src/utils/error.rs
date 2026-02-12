// src/error.rs
use std::fmt;
use std::error::Error as StdError;
use std::io;

/// Custom error type pour rust_engine
#[derive(Debug)]
pub enum EngineError {
    IoError(io::Error),
    SerdeError(serde_json::error::Error),
    TaskError(String),
    SandboxError(String),
    ConfigError(String),
    IntegrationError(String),
    TimeoutError(String),
    PermissionDenied(String),
    Other(String),
}

impl fmt::Display for EngineError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            EngineError::IoError(e) => write!(f, "IO Error: {}", e),
            EngineError::SerdeError(e) => write!(f, "Serialization Error: {}", e),
            EngineError::TaskError(msg) => write!(f, "Task Error: {}", msg),
            EngineError::SandboxError(msg) => write!(f, "Sandbox Error: {}", msg),
            EngineError::ConfigError(msg) => write!(f, "Config Error: {}", msg),
            EngineError::IntegrationError(msg) => write!(f, "Integration Error: {}", msg),
            EngineError::TimeoutError(msg) => write!(f, "Timeout Error: {}", msg),
            EngineError::PermissionDenied(msg) => write!(f, "Permission Denied: {}", msg),
            EngineError::Other(msg) => write!(f, "Error: {}", msg),
        }
    }
}

impl StdError for EngineError {}

impl From<io::Error> for EngineError {
    fn from(err: io::Error) -> Self {
        EngineError::IoError(err)
    }
}

impl From<serde_json::error::Error> for EngineError {
    fn from(err: serde_json::error::Error) -> Self {
        EngineError::SerdeError(err)
    }
}

impl From<String> for EngineError {
    fn from(err: String) -> Self {
        EngineError::Other(err)
    }
}

impl From<&str> for EngineError {
    fn from(err: &str) -> Self {
        EngineError::Other(err.to_string())
    }
}

pub type Result<T> = std::result::Result<T, EngineError>;
