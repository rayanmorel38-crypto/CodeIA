use super::IAIModule;

pub struct SimulateModule;

impl IAIModule for SimulateModule {
    fn name(&self) -> &'static str {
        "simulate"
    }
    fn process(&self, input: &str) -> Result<String, String> {
        Ok(format!("Simulation effectuée sur : {}", input))
    }
}
