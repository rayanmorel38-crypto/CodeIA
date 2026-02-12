// src/engine_modes/ai_rust_hybrid.rs

use crate::core::ai_core::AICore;
use crate::utils::debug_writer::DebugWriter;
use rayon::prelude::*;
use std::sync::Arc;
use tokio::sync::Mutex;
use tokio::task;

/// Structure représentant une tâche hybride
#[derive(Clone, Debug)]
pub struct HybridTask {
    pub id: usize,
    pub content: String,
    pub process_time_ms: u64,
}

impl HybridTask {
    pub fn process(&self) -> String {
        // Simulation d'un travail coûteux
        let mut sum = 0;
        for j in 0..500 {
            sum += j * j;
        }
        format!("Tâche hybride {} : {} (checksum: {})", self.id, self.content, sum)
    }
}

/// Exécution hybride : async + rayon parallel
pub async fn run(task: &str) {
    DebugWriter::info(&format!(
        "[AI Hybrid] Tâche '{}' démarrée en mode hybride",
        task
    ));

    // Phase 1 : Génération en parallèle (Rayon)
    let tasks: Vec<HybridTask> = (0..6)
        .into_par_iter()
        .map(|i| HybridTask {
            id: i,
            content: format!("Proposition {} pour {}", i + 1, task),
            process_time_ms: 50,
        })
        .collect();

    // Phase 2 : Traitement asynchrone des résultats
    let mut handles = vec![];

    for task in tasks {
        let handle = task::spawn(async move {
            let result = task.process();
            DebugWriter::info(&format!("[AI Hybrid] Résultat: {}", result));
            result
        });
        handles.push(handle);
    }

    let mut results = vec![];
    for handle in handles {
        if let Ok(result) = handle.await {
            results.push(result);
        }
    }

    DebugWriter::info(&format!(
        "[AI Hybrid] Tâche terminée - {} résultats générés",
        results.len()
    ));
}

/// Exécution hybride avec AICore
pub async fn run_with_core(task: &str, ai_core: Arc<Mutex<AICore>>) {
    DebugWriter::info(&format!(
        "[AI Hybrid] Exécution hybride de '{}' avec AICore",
        task
    ));

    // Phase parallèle
    let tasks: Vec<HybridTask> = (0..6)
        .into_par_iter()
        .map(|i| HybridTask {
            id: i,
            content: format!("Proposition {} pour {}", i + 1, task),
            process_time_ms: 50,
        })
        .collect();

    // Phase asynchrone
    let mut handles = vec![];

    for hybrid_task in tasks {
        let ai_core = Arc::clone(&ai_core);
        let handle = task::spawn(async move {
            let result = hybrid_task.process();
            
            let mut core = ai_core.lock().await;
            core.integrate_code(result.clone());
            
            DebugWriter::info(&format!("[AI Hybrid] Code intégré: {}", result));
        });
        handles.push(handle);
    }

    // Attendre tous les tasks
    for handle in handles {
        let _ = handle.await;
    }

    let core = ai_core.lock().await;
    let final_code = core.generate_code();
    DebugWriter::info(&format!(
        "[AI Hybrid] Code final généré ({} bytes)",
        final_code.len()
    ));
}

#[cfg(test)]
mod tests {
    use crate::core::ai_core::AICore;
    use std::sync::Arc;
    use tokio::sync::Mutex;

    #[tokio::test]
    async fn test_hybrid_run() {
        let ai_core = Arc::new(Mutex::new(AICore::new()));
        super::run_with_core("test_task", Arc::clone(&ai_core)).await;

        let core = ai_core.lock().await;
        let code = core.generate_code();
        assert!(code.contains("Proposition 1 pour test_task"));
    }

    #[test]
    fn test_hybrid_task() {
        let task = super::HybridTask {
            id: 1,
            content: "Test".to_string(),
            process_time_ms: 50,
        };
        let result = task.process();
        assert!(result.contains("Tâche hybride 1"));
    }
}

