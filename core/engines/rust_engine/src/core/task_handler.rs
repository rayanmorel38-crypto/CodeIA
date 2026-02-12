use crate::core::orchestrator_client::Task;
use crate::utils::error::Result;
use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use tokio::sync::Mutex as TokioMutex;
use std::future::Future;
use std::pin::Pin;

/// Trait synchrone pour les handlers simples
pub trait TaskHandler: Send + Sync {
    fn handle(&self, task: &Task) -> Result<String>;
    fn name(&self) -> &'static str;
}

/// Trait async pour les handlers avec I/O
pub trait AsyncTaskHandler: Send + Sync {
    fn handle_async(&self, task: &Task) -> Pin<Box<dyn Future<Output = Result<String>> + Send>>;
    fn name(&self) -> &'static str;
}

// Registry synchrone (pour les handlers simples)
lazy_static::lazy_static! {
    pub static ref TASK_HANDLER_REGISTRY_SYNC: Mutex<HashMap<String, Arc<dyn TaskHandler>>> = 
        Mutex::new(HashMap::new());
    
    /// Registry async pour les handlers modernes
    pub static ref TASK_HANDLER_REGISTRY_ASYNC: TokioMutex<HashMap<String, Arc<dyn AsyncTaskHandler>>> = 
        TokioMutex::new(HashMap::new());
}

pub fn register_task_handler_sync(handler: Arc<dyn TaskHandler>) {
    let mut reg = TASK_HANDLER_REGISTRY_SYNC.lock().unwrap();
    reg.insert(handler.name().to_string(), handler);
}

pub async fn register_task_handler_async(handler: Arc<dyn AsyncTaskHandler>) {
    let mut reg = TASK_HANDLER_REGISTRY_ASYNC.lock().await;
    reg.insert(handler.name().to_string(), handler);
}

/// Cherche un handler async d'abord, puis fallback sync
pub async fn execute_task_handler(task: &Task) -> Result<String> {
    let handler_name = &task.name;

    // Essayer le registry async d'abord (meilleure performance)
    {
        let async_reg = TASK_HANDLER_REGISTRY_ASYNC.lock().await;
        if let Some(handler) = async_reg.get(handler_name) {
            return handler.handle_async(task).await;
        }
    }

    // Fallback sur le registry sync
    let sync_reg = TASK_HANDLER_REGISTRY_SYNC.lock().unwrap();
    if let Some(handler) = sync_reg.get(handler_name) {
        return handler.handle(task);
    }

    Err(format!("TaskHandler '{}' not found", handler_name).into())
}

#[cfg(test)]
mod tests {
    use super::*;

    struct TestSyncHandler;
    impl TaskHandler for TestSyncHandler {
        fn handle(&self, _: &Task) -> Result<String> {
            Ok("sync handled".to_string())
        }
        fn name(&self) -> &'static str {
            "test_sync"
        }
    }

    struct TestAsyncHandler;
    #[async_trait::async_trait]
    impl AsyncTaskHandler for TestAsyncHandler {
        async fn handle_async(&self, _: &Task) -> Result<String> {
            tokio::time::sleep(tokio::time::Duration::from_millis(10)).await;
            Ok("async handled".to_string())
        }
        fn name(&self) -> &'static str {
            "test_async"
        }
    }

    #[test]
    fn test_sync_handler_registration() {
        let handler = Arc::new(TestSyncHandler);
        register_task_handler_sync(handler);

        let reg = TASK_HANDLER_REGISTRY_SYNC.lock().unwrap();
        assert!(reg.contains_key("test_sync"));
    }

    #[tokio::test]
    async fn test_async_handler_execution() {
        let handler = Arc::new(TestAsyncHandler);
        register_task_handler_async(handler).await;

        let task = Task {
            name: "test_async".to_string(),
            description: "test".to_string(),
            priority: 1,
        };

        let result = execute_task_handler(&task).await;
        assert!(result.is_ok());
        assert_eq!(result.unwrap(), "async handled");
    }
}
