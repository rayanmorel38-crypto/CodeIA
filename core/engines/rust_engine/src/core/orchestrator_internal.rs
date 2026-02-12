// use crate::core::ai::trait_module_ia::TraitModuleIA;
use crate::core::ai::registry::MODULE_IA_REGISTRY;
use serde_json::Value;


pub struct InternalOrchestrator;

impl InternalOrchestrator {
    pub fn process_task(task_type: &str, input: &Value) -> Result<Value, String> {
        let reg = MODULE_IA_REGISTRY.lock().unwrap();
        if let Some(module) = reg.get(task_type) {
            module.process(input)
        } else {
            Err(format!("Aucun module IA pour le type '{}'.", task_type))
        }
    }

    pub fn process_composed(task_types: &[&str], input: &Value) -> Result<Value, String> {
        let mut data = input.clone();
        for &typ in task_types {
            let reg = MODULE_IA_REGISTRY.lock().unwrap();
            if let Some(module) = reg.get(typ) {
                data = module.process(&data)?;
            } else {
                return Err(format!("Module '{}' non trouvé", typ));
            }
        }
        Ok(data)
    }
}
