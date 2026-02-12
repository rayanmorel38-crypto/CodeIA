use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use crate::core::ai::trait_module_ia::TraitModuleIA;

lazy_static::lazy_static! {
    pub static ref MODULE_IA_REGISTRY: Mutex<HashMap<String, Arc<dyn TraitModuleIA>>> = Mutex::new(HashMap::new());
}

pub fn register_module_ia(module: Arc<dyn TraitModuleIA>) {
    let mut reg = MODULE_IA_REGISTRY.lock().unwrap();
    reg.insert(module.name().to_string(), module);
}

pub fn unregister_module_ia(name: &str) {
    let mut reg = MODULE_IA_REGISTRY.lock().unwrap();
    reg.remove(name);
}

pub fn list_modules_ia() -> Vec<String> {
    let reg = MODULE_IA_REGISTRY.lock().unwrap();
    reg.keys().cloned().collect()
}
