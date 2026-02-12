// src/task_runner.rs
use crate::utils::error::{EngineError, Result};
use std::time::Duration;
use tokio::time::timeout;
use std::sync::Arc;

/// Task execution result
#[derive(Debug, Clone)]
pub struct TaskResult {
    pub name: String,
    pub success: bool,
    pub duration_ms: u128,
    pub message: String,
}

/// Task metadata and execution context
pub struct Task {
    pub name: String,
    pub timeout: Option<Duration>,
    pub priority: u8,
}

impl Task {
    pub fn new(name: String, priority: u8) -> Self {
        Task {
            name,
            timeout: None,
            priority,
        }
    }

    pub fn with_timeout(mut self, timeout: Duration) -> Self {
        self.timeout = Some(timeout);
        self
    }
}

/// TaskRunner manages execution of async tasks with timeouts and resource limits
pub struct TaskRunner {
    max_concurrent: usize,
    results: Arc<tokio::sync::Mutex<Vec<TaskResult>>>,
}

impl TaskRunner {
    pub fn new(max_concurrent: usize) -> Self {
        TaskRunner {
            max_concurrent,
            results: Arc::new(tokio::sync::Mutex::new(Vec::new())),
        }
    }

    /// Execute an async function with optional timeout
    pub async fn run<F, Fut>(&self, task: Task, f: F) -> Result<TaskResult>
    where
        F: FnOnce() -> Fut,
        Fut: std::future::Future<Output = Result<String>>,
    {
        let start = std::time::Instant::now();

        let result = if let Some(timeout_duration) = task.timeout {
            match timeout(timeout_duration, f()).await {
                Ok(Ok(msg)) => Ok(msg),
                Ok(Err(e)) => Err(e),
                Err(_) => Err(EngineError::TimeoutError(format!(
                    "Task '{}' exceeded timeout of {:?}",
                    task.name, timeout_duration
                ))),
            }
        } else {
            f().await
        };

        let duration_ms = start.elapsed().as_millis();
        let (success, message) = match result {
            Ok(msg) => (true, msg),
            Err(e) => (false, e.to_string()),
        };

        let task_result = TaskResult {
            name: task.name.clone(),
            success,
            duration_ms,
            message,
        };

        self.results.lock().await.push(task_result.clone());
        Ok(task_result)
    }

    /// Get all collected results
    pub async fn get_results(&self) -> Vec<TaskResult> {
        self.results.lock().await.clone()
    }

    /// Get the maximum concurrent tasks
    pub fn max_concurrent(&self) -> usize {
        self.max_concurrent
    }

    /// Get summary of results
    pub async fn get_summary(&self) -> String {
        let results = self.results.lock().await;
        let total = results.len();
        let succeeded = results.iter().filter(|r| r.success).count();
        let failed = total - succeeded;
        let total_time: u128 = results.iter().map(|r| r.duration_ms).sum();

        format!(
            "TaskRunner Summary: {} total, {} succeeded, {} failed, {} ms total",
            total, succeeded, failed, total_time
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_task_execution() {
        let runner = TaskRunner::new(4);
        let task = Task::new("test_task".to_string(), 0);
        let result = runner
            .run(task, || async { Ok("success".to_string()) })
            .await;

        assert!(result.is_ok());
        let r = result.unwrap();
        assert!(r.success);
        assert_eq!(r.message, "success");
    }

    #[tokio::test]
    async fn test_task_timeout() {
        let runner = TaskRunner::new(4);
        let task = Task::new("timeout_task".to_string(), 0)
            .with_timeout(Duration::from_millis(100));

        let result = runner
            .run(task, || async {
                tokio::time::sleep(Duration::from_secs(1)).await;
                Ok("should not reach".to_string())
            })
            .await;

        assert!(result.is_ok());
        let r = result.unwrap();
        assert!(!r.success); // Task timed out
    }
}
