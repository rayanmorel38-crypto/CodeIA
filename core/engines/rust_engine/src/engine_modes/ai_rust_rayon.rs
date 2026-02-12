// src/engine_modes/ai_rust_rayon.rs

use crate::core::ai_core::AICore;
use crate::utils::debug_writer::DebugWriter;
use rayon::prelude::*;
use std::sync::Arc;
use tokio::sync::Mutex;

/// Structure représentant une proposition ou tâche à traiter
#[derive(Clone, Debug)]
pub struct Proposal {
    pub id: usize,
    pub content: String,
}

impl Proposal {
    pub fn process(&self) -> String {
        // Simulation d'un traitement coûteux
        let mut sum = 0;
        for j in 0..1000 {
            sum += j;
        }
        format!("Proposition {} : {} (checksum: {})", self.id, self.content, sum)
    }
}

/// Exécution parallèle via Rayon pour du vrai traitement parallèle
pub fn run(task: &str) {
    DebugWriter::info(&format!(
        "[AI Rayon] Tâche '{}' démarrée en mode parallèle",
        task
    ));

    let proposals: Vec<Proposal> = (0..8)
        .map(|i| Proposal {
            id: i,
            content: format!("Proposition {} pour {}", i + 1, task),
        })
        .collect();

    let processed: Vec<_> = proposals
        .into_par_iter()
        .map(|p| {
            let result = p.process();
            DebugWriter::info(&format!("[AI Rayon] Traité: {}", result));
            result
        })
        .collect();

    DebugWriter::info(&format!(
        "[AI Rayon] Tâche terminée - {} propositions traitées en parallèle",
        processed.len()
    ));
}

/// Exécution parallèle avec AICore
pub async fn run_with_core(task: &str, ai_core: Arc<Mutex<AICore>>) {
    DebugWriter::info(&format!("[AI Rayon] Exécution avec AICore pour '{}'", task));
    
    let proposals: Vec<Proposal> = (0..8)
        .map(|i| Proposal {
            id: i,
            content: format!("Proposition {} pour {}", i + 1, task),
        })
        .collect();

    // Processus parallèle dans un bloc blocking
    let ai_core_clone = Arc::clone(&ai_core);
    tokio::task::spawn_blocking(move || {
        let processed: Vec<_> = proposals
            .into_par_iter()
            .map(|p| {
                let result = p.process();
                DebugWriter::info(&format!("[AI Rayon] Traité: {}", result));
                result
            })
            .collect();

        // Intégration dans AICore
        let core_handle = ai_core_clone.blocking_lock();
        let mut core = core_handle;
        for result in processed {
            core.integrate_code(result);
        }
    })
    .await
    .unwrap();

    let core = ai_core.lock().await;
    let final_code = core.generate_code();
    DebugWriter::info(&format!(
        "[AI Rayon] Code final généré ({} bytes)",
        final_code.len()
    ));
}

#[cfg(test)]
mod tests {
    use crate::core::ai_core::AICore;
    use std::sync::Arc;
    use tokio::sync::Mutex;

    #[tokio::test]
    async fn test_rayon_run() {
        let ai_core = Arc::new(Mutex::new(AICore::new()));
        super::run_with_core("test_task", Arc::clone(&ai_core)).await;

        let core = ai_core.lock().await;
        let code = core.generate_code();
        assert!(code.contains("Proposition 1 pour test_task"));
    }

    #[test]
    fn test_proposal_processing() {
        let proposal = super::Proposal {
            id: 1,
            content: "Test".to_string(),
        };
        let result = proposal.process();
        assert!(result.contains("Proposition 1"));
    }
}

