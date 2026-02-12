use super::IAIModule;

pub struct AnalyzeModule;

impl IAIModule for AnalyzeModule {
    fn name(&self) -> &'static str {
        "analyze"
    }
    fn process(&self, input: &str) -> Result<String, String> {
        Ok(format!("Analyse effectuée sur : {}", input))
    }
}
