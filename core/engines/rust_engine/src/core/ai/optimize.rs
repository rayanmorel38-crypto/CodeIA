use super::IAIModule;

pub struct OptimizeModule;

impl IAIModule for OptimizeModule {
    fn name(&self) -> &'static str {
        "optimize"
    }
    fn process(&self, input: &str) -> Result<String, String> {
        Ok(format!("Optimisation effectuée sur : {}", input))
    }
}
