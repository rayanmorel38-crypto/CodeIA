// src/engine_modes/ai_rust_async.rs

use crate::core::ai_core::AICore;
use crate::utils::debug_writer::DebugWriter;
use std::sync::Arc;
use tokio::{sync::Mutex, task, time};
use std::time::Duration;

/// Exécute une tâche de manière entièrement asynchrone avec concurrence réelle
pub async fn run(task: &str) {
    DebugWriter::info(&format!("[AI Async] Tâche '{}' démarrée en mode asynchrone", task));

    // Créer des tâches asynchrones indépendantes
    let mut handles = vec![];
    
    for i in 0..3 {
        let task_name = task.to_string();
        let handle = task::spawn(async move {
            // Simuler un travail asynchrone avec délai
            time::sleep(Duration::from_millis(100 * (i as u64 + 1))).await;
            
            let result = format!("Proposition {} pour {}", i + 1, task_name);
            DebugWriter::info(&format!("[AI Async] {} (terminée après {}ms)", result, 100 * (i + 1)));
            result
        });
        handles.push(handle);
    }

    // Attendre tous les tasks de manière concurrente
    let mut results = vec![];
    for handle in handles {
        if let Ok(result) = handle.await {
            results.push(result);
        }
    }

    DebugWriter::info(&format!(
        "[AI Async] Tâche terminée - {} résultats générés",
        results.len()
    ));
}

/// Exécution async avec intégration dans AICore
pub async fn run_with_core(task: &str, ai_core: Arc<Mutex<AICore>>) {
    DebugWriter::info(&format!("[AI Async] Exécution de '{}' avec AICore", task));
    
    let mut handles = vec![];
    
    for i in 0..3 {
        let ai_core = Arc::clone(&ai_core);
        let task_name = task.to_string();
        
        let handle = task::spawn(async move {
            // Délai asynchrone avant traitement
            time::sleep(Duration::from_millis(50 * (i as u64 + 1))).await;
            
            let mut core = ai_core.lock().await;
            let code = format!("async fn task_{}() {{\n    println!(\"{}\\n\");\n}}", i, task_name);
            core.integrate_code(code.clone());
            DebugWriter::info(&format!("[AI Async] Code intégré dans AICore pour proposition {}", i + 1));
        });
        
        handles.push(handle);
    }

    // Attendre la complétion de tous les handles
    for handle in handles {
        let _ = handle.await;
    }

    // Générer le code final
    let core = ai_core.lock().await;
    let final_code = core.generate_code();
    DebugWriter::info(&format!("[AI Async] Code final généré ({}  bytes)", final_code.len()));
}

#[cfg(test)]
mod tests {
    use crate::core::ai_core::AICore;
    use std::sync::Arc;
    use tokio::sync::Mutex;

    #[tokio::test]
    async fn test_async_run() {
        let ai_core = Arc::new(Mutex::new(AICore::new()));
        super::run_with_core("async_test", Arc::clone(&ai_core)).await;

        let core = ai_core.lock().await;
        let code = core.generate_code();
        // Correspond exactement au format des logs
        assert!(code.contains("Proposition 1 pour async_test"));
    }
}

