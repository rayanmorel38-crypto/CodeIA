use serde_json::Value;

pub trait TraitModuleIA: Send + Sync {
    fn name(&self) -> &'static str;
    fn capabilities(&self) -> Vec<&'static str>;
    fn process(&self, input: &Value) -> Result<Value, String>;
}
