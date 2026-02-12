pub mod analyze;
pub mod optimize;
pub mod simulate;
pub mod plan;
pub mod trait_module_ia;
pub mod registry;

pub use crate::core::ai::analyze::AnalyzeModule;
pub use crate::core::ai::optimize::OptimizeModule;
pub use crate::core::ai::simulate::SimulateModule;
pub use crate::core::ai::plan::PlanModule;

pub trait IAIModule: Send + Sync {
    fn name(&self) -> &'static str;
    fn process(&self, input: &str) -> Result<String, String>;
}

// Registre global des modules IA dynamiques
use std::collections::HashMap;
use std::sync::{Arc, Mutex};
lazy_static::lazy_static! {
    pub static ref IA_MODULE_REGISTRY: Mutex<HashMap<String, Arc<dyn IAIModule>>> = Mutex::new(HashMap::new());
}

pub fn register_module(module: Arc<dyn IAIModule>) {
    let mut reg = IA_MODULE_REGISTRY.lock().unwrap();
    reg.insert(module.name().to_string(), module);
}
