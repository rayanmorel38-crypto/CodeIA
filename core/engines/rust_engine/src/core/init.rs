use crate::core::ai_core::{AICore, Role};
use crate::engine_modes::{ai_rust_async, ai_rust_autonomous, ai_rust_hybrid, ai_rust_rayon};
use crate::core::sandbox::Sandbox;
use crate::core::ipc::IPC;
use crate::utils::debug_writer::DebugWriter;
use std::sync::Arc;
use tokio::sync::Mutex;

pub async fn run_self_test() {
    DebugWriter::info("=== Initialisation et self-test IA ===");

    let ai_core = Arc::new(Mutex::new(AICore::new()));
    let roles = vec![Role::Developer, Role::Analyst];

    for role in &roles {
        DebugWriter::info(&format!("Rôle assigné pour self-test : {:?}", role));
    }

    let sb = Sandbox::new().expect("Impossible de créer la sandbox pour self-test");
    sb.log_task("Préparation des modules IA").unwrap();

    // --- Initialisation de IPC avec nouvelle API ---
    let ipc = Arc::new(IPC::new_async());
    DebugWriter::info("Canal IPC initialisé pour le self-test");

    ipc.send_normal("Démarrage du self-test IA via IPC".to_string()).await;

    // Async
    let ai_core_clone = Arc::clone(&ai_core);
    ai_rust_async::run_with_core("selftest_async", ai_core_clone).await;
    sb.log_task("AI Async self-test initialisé").unwrap();
    ipc.send_normal("AI Async OK".to_string()).await;

    // Autonomous
    let ai_core_clone2 = Arc::clone(&ai_core);
    ai_rust_autonomous::run_with_core("selftest_autonomous", ai_core_clone2).await;
    sb.log_task("AI Autonomous self-test initialisé").unwrap();
    ipc.send_normal("AI Autonomous OK".to_string()).await;

    // Hybrid
    let ai_core_clone3 = Arc::clone(&ai_core);
    ai_rust_hybrid::run_with_core("selftest_hybrid", ai_core_clone3).await;
    sb.log_task("AI Hybrid self-test initialisé").unwrap();
    ipc.send_normal("AI Hybrid OK".to_string()).await;

    // Rayon
    let ai_core_clone4 = Arc::clone(&ai_core);
    ai_rust_rayon::run_with_core("selftest_rayon", ai_core_clone4).await;
    sb.log_task("AI Rayon self-test initialisé").unwrap();
    ipc.send_normal("AI Rayon OK".to_string()).await;

    ipc.send_normal("Self-test complet terminé".to_string()).await;
    DebugWriter::info("=== Self-test IA terminé ===");
}

