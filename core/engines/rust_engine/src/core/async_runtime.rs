/// Gestion avancée du runtime async pour le moteur IA
use tokio::sync::Semaphore;
use std::sync::Arc;
use std::time::Duration;

/// Configuration du runtime async
#[derive(Clone, Debug)]
pub struct AsyncConfig {
    pub worker_threads: usize,
    pub max_blocking_threads: usize,
    pub event_loop_capacity: usize,
    pub timeout_ms: u64,
}

impl Default for AsyncConfig {
    fn default() -> Self {
        AsyncConfig {
            worker_threads: num_cpus::get(),
            max_blocking_threads: 512,
            event_loop_capacity: 10000,
            timeout_ms: 30000,
        }
    }
}

/// Pool de connexions asynchrones
pub struct AsyncPool {
    config: AsyncConfig,
    semaphore: Arc<Semaphore>,
}

impl AsyncPool {
    pub fn new(config: AsyncConfig) -> Self {
        let semaphore = Arc::new(Semaphore::new(config.max_blocking_threads));
        AsyncPool { config, semaphore }
    }

    pub async fn execute<F, T>(&self, f: F) -> Result<T, String>
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        let _permit = self
            .semaphore
            .acquire()
            .await
            .map_err(|e| format!("Failed to acquire permit: {}", e))?;

        tokio::task::spawn_blocking(f)
            .await
            .map_err(|e| format!("Task join error: {}", e))
    }

    pub async fn execute_with_timeout<F, T>(&self, f: F, timeout_ms: u64) -> Result<T, String>
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        let _permit = self
            .semaphore
            .acquire()
            .await
            .map_err(|e| format!("Failed to acquire permit: {}", e))?;

        tokio::time::timeout(
            Duration::from_millis(timeout_ms),
            tokio::task::spawn_blocking(f),
        )
        .await
        .map_err(|_| "Task timeout".to_string())?
        .map_err(|e| format!("Task join error: {}", e))
    }

    pub fn config(&self) -> &AsyncConfig {
        &self.config
    }
}

/// Gestionnaire des tâches asynchrones globales
pub struct AsyncTaskManager {
    pool: Arc<AsyncPool>,
    active_tasks: Arc<tokio::sync::Mutex<usize>>,
    completed_tasks: Arc<tokio::sync::Mutex<usize>>,
}

impl AsyncTaskManager {
    pub fn new(config: AsyncConfig) -> Self {
        AsyncTaskManager {
            pool: Arc::new(AsyncPool::new(config)),
            active_tasks: Arc::new(tokio::sync::Mutex::new(0)),
            completed_tasks: Arc::new(tokio::sync::Mutex::new(0)),
        }
    }

    pub async fn spawn<F, T>(&self, f: F) -> Result<T, String>
    where
        F: FnOnce() -> T + Send + 'static,
        T: Send + 'static,
    {
        let mut active = self.active_tasks.lock().await;
        *active += 1;

        let result = self.pool.execute(f).await;

        let mut completed = self.completed_tasks.lock().await;
        *completed += 1;
        *active -= 1;

        result
    }

    pub async fn get_stats(&self) -> (usize, usize) {
        let active = *self.active_tasks.lock().await;
        let completed = *self.completed_tasks.lock().await;
        (active, completed)
    }
}

lazy_static::lazy_static! {
    pub static ref GLOBAL_ASYNC_MANAGER: AsyncTaskManager = 
        AsyncTaskManager::new(AsyncConfig::default());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_async_pool_execution() {
        let config = AsyncConfig::default();
        let pool = AsyncPool::new(config);

        let result = pool
            .execute(|| {
                std::thread::sleep(Duration::from_millis(10));
                42
            })
            .await;

        assert!(result.is_ok());
        assert_eq!(result.unwrap(), 42);
    }

    #[tokio::test]
    async fn test_async_pool_timeout() {
        let config = AsyncConfig::default();
        let pool = AsyncPool::new(config);

        let result = pool
            .execute_with_timeout(
                || {
                    std::thread::sleep(Duration::from_secs(10));
                    42
                },
                100,
            )
            .await;

        assert!(result.is_err());
    }

    #[tokio::test]
    async fn test_task_manager_stats() {
        let manager = AsyncTaskManager::new(AsyncConfig::default());

        let (active, completed) = manager.get_stats().await;
        assert_eq!(active, 0);
        assert_eq!(completed, 0);
    }
}
