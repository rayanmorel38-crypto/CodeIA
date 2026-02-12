use super::IAIModule;

pub struct PlanModule;

impl IAIModule for PlanModule {
    fn name(&self) -> &'static str {
        "plan"
    }
    fn process(&self, input: &str) -> Result<String, String> {
        Ok(format!("Planification effectuée sur : {}", input))
    }
}
