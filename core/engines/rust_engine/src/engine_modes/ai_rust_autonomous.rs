// src/engine_modes/ai_rust_autonomous.rs

use crate::core::ai_core::AICore;
use crate::utils::debug_writer::DebugWriter;
use std::sync::Arc;
use tokio::sync::Mutex;

/// État d'exécution autonome
#[derive(Clone, Debug)]
pub struct AutonomousState {
    pub current_iteration: usize,
    pub max_iterations: usize,
    pub goal: String,
    pub results: Vec<String>,
}

impl AutonomousState {
    pub fn new(goal: String, max_iterations: usize) -> Self {
        AutonomousState {
            current_iteration: 0,
            max_iterations,
            goal,
            results: Vec::new(),
        }
    }

    pub fn step(&mut self) -> bool {
        if self.current_iteration >= self.max_iterations {
            return false;
        }

        let result = format!(
            "Itération {} vers l'objectif: {}",
            self.current_iteration + 1,
            self.goal
        );
        self.results.push(result);
        self.current_iteration += 1;
        true
    }
}

/// Exécute une tâche en mode autonome (boucle itérative)
pub fn run(task: &str) {
    DebugWriter::info(&format!("[AI Autonomous] Tâche '{}' démarrée", task));

    let mut state = AutonomousState::new(task.to_string(), 3);

    while state.step() {
        DebugWriter::info(&format!(
            "[AI Autonomous] Itération {} - Objectif: {}",
            state.current_iteration, state.goal
        ));
    }

    DebugWriter::info(&format!(
        "[AI Autonomous] Tâche terminée après {} itérations",
        state.current_iteration
    ));
}

/// Exécution autonome avec AICore
pub async fn run_with_core(task: &str, ai_core: Arc<Mutex<AICore>>) {
    DebugWriter::info(&format!(
        "[AI Autonomous] Exécution autonome de '{}' avec AICore",
        task
    ));

    let mut core = ai_core.lock().await;
    let mut state = AutonomousState::new(task.to_string(), 3);

    while state.step() {
        let code = format!(
            "fn autonomous_iteration_{}() {{\n    // Objectif: {}\n    println!(\"Itération {}\\n\");\n}}",
            state.current_iteration, state.goal, state.current_iteration
        );

        core.integrate_code(code.clone());
        DebugWriter::info(&format!(
            "[AI Autonomous] Code intégré pour itération {}",
            state.current_iteration
        ));
    }

    DebugWriter::info(&format!(
        "[AI Autonomous] {} itérations complétées",
        state.current_iteration
    ));
}

#[cfg(test)]
mod tests {
    use crate::core::ai_core::AICore;
    use std::sync::Arc;
    use tokio::sync::Mutex;

    #[tokio::test]
    async fn test_autonomous_run() {
        let ai_core = Arc::new(Mutex::new(AICore::new()));
        super::run_with_core("auto_test", Arc::clone(&ai_core)).await;

        let core = ai_core.lock().await;
        let code = core.generate_code();
        assert!(code.contains("auto_test"));
    }

    #[test]
    fn test_autonomous_state() {
        let mut state = super::AutonomousState::new("test".to_string(), 3);
        assert_eq!(state.current_iteration, 0);

        state.step();
        assert_eq!(state.current_iteration, 1);
        assert_eq!(state.results.len(), 1);
    }
}

