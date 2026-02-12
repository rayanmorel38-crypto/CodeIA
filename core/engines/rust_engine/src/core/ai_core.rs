// src/core/ai_core.rs

use crate::utils::debug_writer::DebugWriter;
use sha2::{Sha256, Digest};
use tokio::sync::Mutex;
use std::sync::Arc;
use crate::core::orchestrator_client::Task;
use crate::core::sandbox::Sandbox;
use crate::utils::error::Result;

/// Définition des rôles de l’IA
#[derive(Clone, Debug)]
pub enum Role {
    Analyst,
    Developer,
    Tester,
    Learner,
    Admin, // ajouté pour compatibilité main.rs
}

/// Modes d’exécution de l’IA
#[derive(Clone, Debug)]
pub enum Mode {
    Autonomous,
    Async,
    Hybrid,
    Rayon,
}

/// Core principal de l’IA
pub struct AICore {
    code_storage: Vec<String>,
    roles: Vec<Role>,
}

impl AICore {
    pub fn new() -> Self {
        use crate::core::ai::{register_module, AnalyzeModule, OptimizeModule, SimulateModule, PlanModule};
        use std::sync::Arc;
        // Enregistrement des modules IA de base
        register_module(Arc::new(AnalyzeModule));
        register_module(Arc::new(OptimizeModule));
        register_module(Arc::new(SimulateModule));
        register_module(Arc::new(PlanModule));
        AICore {
            code_storage: Vec::new(),
            roles: Vec::new(),
        }
    }

    /// Attribuer des rôles à l’IA
    pub fn set_roles(&mut self, roles: Vec<Role>) {
        self.roles = roles;
        DebugWriter::info("Rôles définis dans AICore");
    }

    /// Récupérer les rôles
    pub fn get_roles(&self) -> &[Role] {
        &self.roles
    }

    /// Exécuter une tâche reçue de l'orchestrateur
    pub async fn execute_task(&self, task: &Task, _roles: &[Role], _sandbox: &Sandbox) -> Result<String> {
        use crate::core::task_handler_sync::TASK_HANDLER_REGISTRY_SYNC;
        use crate::core::ai::IA_MODULE_REGISTRY;
        DebugWriter::info(&format!("Exécution de la tâche: {} (type: {})", task.name, task.task_type));

        // 1. Handler synchrone dynamique
        let reg = TASK_HANDLER_REGISTRY_SYNC.lock().unwrap();
        if let Some(handler) = reg.get(&task.task_type) {
            return handler.handle(task);
        }
        drop(reg);

        // 2. Fallback IA module (pour compatibilité IA)
        let ia_reg = IA_MODULE_REGISTRY.lock().unwrap();
        if let Some(module) = ia_reg.get(&task.task_type) {
            let input = match &task.data {
                serde_json::Value::String(s) => s.clone(),
                _ => task.data.to_string(),
            };
            match module.process(&input) {
                Ok(res) => Ok(res),
                Err(e) => Err(crate::utils::error::EngineError::IntegrationError(format!("Erreur module IA '{}': {}", task.task_type, e)))
            }
        } else {
            Err(crate::utils::error::EngineError::IntegrationError(format!("Type de tâche inconnu: {}", task.task_type)))
        }
    }

    /// Intègre du code dans le core
    pub fn integrate_code(&mut self, code: String) {
        self.code_storage.push(code);
        DebugWriter::info("Code intégré dans AICore");
    }

    /// Génère une concaténation de tout le code intégré
    pub fn generate_code(&self) -> String {
        self.code_storage.join("\n")
    }

    /// Fonction utilitaire pour hasher des bytes
    pub fn sha256_bytes(data: &[u8]) -> String {
        let mut hasher = Sha256::new();
        hasher.update(data);
        format!("{:x}", hasher.finalize())
    }
}

/// Wrapper AIRust pour gérer le core et les rôles
pub struct AIRust {
    pub core: Arc<Mutex<AICore>>,
}

impl AIRust {
    pub fn new() -> Self {
        AIRust {
            core: Arc::new(Mutex::new(AICore::new())),
        }
    }

    pub async fn set_roles(&self, roles: Vec<Role>) {
        let mut core = self.core.lock().await;
        core.set_roles(roles);
    }

    pub async fn run_cycle(&self) -> Vec<String> {
        let core = self.core.lock().await;
        core.code_storage.clone()
    }

    pub async fn generate_code(&self) -> String {
        self.core.lock().await.generate_code()
    }

    pub async fn integrate_code(&self, code: String) {
        let mut core = self.core.lock().await;
        core.integrate_code(code);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_integrate_and_generate() {
        let mut core = AICore::new();
        core.integrate_code("let x = 1;".to_string());
        core.integrate_code("let y = 2;".to_string());
        let code = core.generate_code();
        assert!(code.contains("let x = 1;"));
        assert!(code.contains("let y = 2;"));
    }

    #[test]
    fn test_sha256_bytes() {
        let hash = AICore::sha256_bytes(b"test");
        assert_eq!(
            hash,
            "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"
        );
    }

    #[test]
    fn test_roles() {
        let mut core = AICore::new();
        core.set_roles(vec![Role::Analyst, Role::Developer, Role::Admin]);
        let roles = core.get_roles();
        assert_eq!(roles.len(), 3);
    }

    #[tokio::test]
    async fn test_airust_wrapper() {
        let ai = AIRust::new();
        ai.set_roles(vec![Role::Developer, Role::Admin]).await;
        ai.integrate_code("fn main() {}".to_string()).await;
        let code = ai.generate_code().await;
        assert!(code.contains("fn main() {}"));
    }
}

