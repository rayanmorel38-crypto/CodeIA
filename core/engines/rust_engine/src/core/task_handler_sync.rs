use crate::core::orchestrator_client::Task;
use crate::utils::error::Result;

pub trait TaskHandlerSync: Send + Sync {
    fn handle(&self, task: &Task) -> Result<String>;
    fn name(&self) -> &'static str;
}

use std::collections::HashMap;
use std::sync::{Arc, Mutex};
lazy_static::lazy_static! {
    pub static ref TASK_HANDLER_REGISTRY_SYNC: Mutex<HashMap<String, Arc<dyn TaskHandlerSync>>> = Mutex::new(HashMap::new());
}

pub fn register_task_handler_sync(handler: Arc<dyn TaskHandlerSync>) {
    let mut reg = TASK_HANDLER_REGISTRY_SYNC.lock().unwrap();
    reg.insert(handler.name().to_string(), handler);
}
