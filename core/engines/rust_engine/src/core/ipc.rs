// src/ipc.rs

use crate::core::ai_core::AICore;
use crate::utils::debug_writer::DebugWriter;
use std::sync::Arc;
use tokio::sync::{mpsc, Mutex};
use std::collections::{BinaryHeap, VecDeque};
use std::cmp::Reverse;
use serde::{Serialize, Deserialize};

/// Message avec priorité pour l'IPC
#[derive(Clone, Debug, Serialize, Deserialize, Eq, PartialEq)]
pub struct PriorityMessage {
    pub id: u64,
    pub content: String,
    pub priority: u32, // 0=basse, 100=critique
    pub timestamp: u64,
}

impl Ord for PriorityMessage {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        // Ordonner par priorité décroissante, puis par timestamp
        other.priority
            .cmp(&self.priority)
            .then_with(|| self.timestamp.cmp(&other.timestamp))
    }
}

impl PartialOrd for PriorityMessage {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

/// IPC amélioreé avec queue de priorité et persistence
pub struct IPC {
    pub sender: mpsc::Sender<PriorityMessage>,
    pub message_queue: Arc<Mutex<BinaryHeap<Reverse<PriorityMessage>>>>,
    pub delivery_log: Arc<Mutex<VecDeque<PriorityMessage>>>,
    pub max_queue_size: usize,
    pub message_counter: Arc<Mutex<u64>>,
}

impl Clone for IPC {
    fn clone(&self) -> Self {
        IPC {
            sender: self.sender.clone(),
            message_queue: Arc::clone(&self.message_queue),
            delivery_log: Arc::clone(&self.delivery_log),
            max_queue_size: self.max_queue_size,
            message_counter: Arc::clone(&self.message_counter),
        }
    }
}

impl IPC {
    /// Crée un IPC avec queue de priorité
    pub fn new(capacity: usize, max_queue_size: usize) -> (Self, mpsc::Receiver<PriorityMessage>) {
        let (tx, rx) = mpsc::channel(capacity);
        
        (
            IPC {
                sender: tx,
                message_queue: Arc::new(Mutex::new(BinaryHeap::new())),
                delivery_log: Arc::new(Mutex::new(VecDeque::with_capacity(1000))),
                max_queue_size,
                message_counter: Arc::new(Mutex::new(0)),
            },
            rx,
        )
    }

    /// Crée un IPC async simplifiée (sans channel receiver)
    pub fn new_async() -> Self {
        let (tx, _) = mpsc::channel(100);
        
        IPC {
            sender: tx,
            message_queue: Arc::new(Mutex::new(BinaryHeap::new())),
            delivery_log: Arc::new(Mutex::new(VecDeque::with_capacity(1000))),
            max_queue_size: 1000,
            message_counter: Arc::new(Mutex::new(0)),
        }
    }

    /// Envoi de message avec priorité
    pub async fn send(&self, content: String, priority: u32) {
        let mut counter = self.message_counter.lock().await;
        *counter += 1;

        let msg = PriorityMessage {
            id: *counter,
            content,
            priority,
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap_or_default()
                .as_secs(),
        };

        // Ajouter à la queue de priorité
        let mut queue = self.message_queue.lock().await;
        if queue.len() < self.max_queue_size {
            queue.push(Reverse(msg.clone()));
        }

        // Envoyer au canal
        let _ = self.sender.send(msg).await;
    }

    /// Envoi urgent (priorité critique)
    pub async fn send_critical(&self, content: String) {
        self.send(content, 100).await;
    }

    /// Envoi normal (priorité moyenne)
    pub async fn send_normal(&self, content: String) {
        self.send(content, 50).await;
    }

    /// Envoi basse priorité
    pub async fn send_low(&self, content: String) {
        self.send(content, 10).await;
    }

    /// Récupère le message de plus haute priorité
    pub async fn get_highest_priority(&self) -> Option<PriorityMessage> {
        let mut queue = self.message_queue.lock().await;
        queue.pop().map(|Reverse(msg)| msg)
    }

    /// Boucle de réception AI avec gestion de priorité
    pub async fn run_ai_loop(
        receiver: &mut mpsc::Receiver<PriorityMessage>,
        ai_core: Arc<Mutex<AICore>>,
        ipc: Arc<IPC>,
    ) {
        while let Some(msg) = receiver.recv().await {
            DebugWriter::info(&format!(
                "Message reçu (ID: {}, Priorité: {}, Contenu: {})",
                msg.id, msg.priority, msg.content
            ));

            let mut core = ai_core.lock().await;
            core.integrate_code(msg.content.clone());

            // Ajouter au log de livraison
            let mut log = ipc.delivery_log.lock().await;
            log.push_back(msg);
            if log.len() > 1000 {
                log.pop_front();
            }
        }
    }

    /// Récupère l'historique des messages livrés
    pub async fn get_delivery_log(&self) -> Vec<PriorityMessage> {
        self.delivery_log.lock().await.iter().cloned().collect()
    }

    /// Récupère la taille actuelle de la queue
    pub async fn queue_size(&self) -> usize {
        self.message_queue.lock().await.len()
    }

    /// Récupère le nombre de messages traités
    pub async fn get_message_count(&self) -> u64 {
        *self.message_counter.lock().await
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_priority_ordering() {
        let (ipc, mut rx) = IPC::new(100, 100);

        ipc.send_low("Low priority".to_string()).await;
        ipc.send_critical("Critical".to_string()).await;
        ipc.send_normal("Normal".to_string()).await;

        // Les messages devraient être reçus par ordre de priorité
        let msg1 = rx.recv().await.unwrap();
        let msg2 = rx.recv().await.unwrap();
        let msg3 = rx.recv().await.unwrap();

        assert!(msg1.priority >= msg2.priority);
        assert!(msg2.priority >= msg3.priority);
    }

    #[tokio::test]
    async fn test_queue_operations() {
        let (ipc, _rx) = IPC::new(100, 100);

        ipc.send_normal("Message 1".to_string()).await;
        ipc.send_normal("Message 2".to_string()).await;

        assert_eq!(ipc.queue_size().await, 2);
        assert_eq!(ipc.get_message_count().await, 2);
    }
}

