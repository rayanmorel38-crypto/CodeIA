// src/modules/complex_state_management.rs

use std::collections::{HashMap, VecDeque};
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use crate::utils::debug_writer::DebugWriter;

/// État complexe avec propriétés dynamiques
#[derive(Clone, Debug)]
pub struct ComplexState {
    pub id: String,
    pub properties: HashMap<String, StateValue>,
    pub metadata: HashMap<String, String>,
    pub created_at: Instant,
    pub updated_at: Instant,
    pub version: u64,
}

impl ComplexState {
    pub fn new(id: String) -> Self {
        let now = Instant::now();
        ComplexState {
            id,
            properties: HashMap::new(),
            metadata: HashMap::new(),
            created_at: now,
            updated_at: now,
            version: 1,
        }
    }

    /// Définit une propriété
    pub fn set_property(&mut self, key: String, value: StateValue) {
        self.properties.insert(key, value);
        self.updated_at = Instant::now();
        self.version += 1;
    }

    /// Obtient une propriété
    pub fn get_property(&self, key: &str) -> Option<&StateValue> {
        self.properties.get(key)
    }

    /// Supprime une propriété
    pub fn remove_property(&mut self, key: &str) -> bool {
        let removed = self.properties.remove(key).is_some();
        if removed {
            self.updated_at = Instant::now();
            self.version += 1;
        }
        removed
    }

    /// Vérifie si l'état a changé depuis une version donnée
    pub fn has_changed_since(&self, version: u64) -> bool {
        self.version > version
    }

    /// Durée depuis la dernière mise à jour
    pub fn time_since_update(&self) -> Duration {
        self.updated_at.elapsed()
    }
}

/// Valeur d'une propriété d'état
#[derive(Clone, Debug, PartialEq)]
pub enum StateValue {
    Integer(i64),
    Float(f64),
    String(String),
    Boolean(bool),
    List(Vec<StateValue>),
    Map(HashMap<String, StateValue>),
}

/// Gestionnaire d'états avec historique et transactions
pub struct StateManager {
    states: HashMap<String, ComplexState>,
    history: VecDeque<StateSnapshot>,
    max_history_size: usize,
    transactions: Vec<StateTransaction>,
    #[allow(dead_code)]
    debug_writer: Arc<Mutex<DebugWriter>>,
}

impl StateManager {
    pub fn new(max_history_size: usize, debug_writer: Arc<Mutex<DebugWriter>>) -> Self {
        StateManager {
            states: HashMap::new(),
            history: VecDeque::with_capacity(max_history_size),
            max_history_size,
            transactions: Vec::new(),
            debug_writer,
        }
    }

    /// Crée un nouvel état
    pub fn create_state(&mut self, id: String) -> Result<(), StateError> {
        if self.states.contains_key(&id) {
            return Err(StateError::StateAlreadyExists(id));
        }

        let state = ComplexState::new(id.clone());
        self.states.insert(id.clone(), state);
        self.take_snapshot("create_state");

        // Log the creation
        DebugWriter::info(&format!("Created state: {}", id));

        Ok(())
    }

    /// Met à jour un état
    pub fn update_state(&mut self, id: &str, updates: HashMap<String, StateValue>) -> Result<(), StateError> {
        let state = self.states.get_mut(id).ok_or_else(|| StateError::StateNotFound(id.to_string()))?;

        for (key, value) in updates {
            state.set_property(key, value);
        }

        self.take_snapshot("update_state");

        // Log the update
        DebugWriter::info(&format!("Updated state: {}", id));

        Ok(())
    }

    /// Supprime un état
    pub fn delete_state(&mut self, id: &str) -> Result<(), StateError> {
        if self.states.remove(id).is_none() {
            return Err(StateError::StateNotFound(id.to_string()));
        }

        self.take_snapshot("delete_state");

        // Log the deletion
        DebugWriter::info(&format!("Deleted state: {}", id));

        Ok(())
    }

    /// Obtient un état
    pub fn get_state(&self, id: &str) -> Option<&ComplexState> {
        self.states.get(id)
    }

    /// Recherche d'états par critères
    pub fn query_states(&self, predicate: &dyn Fn(&ComplexState) -> bool) -> Vec<&ComplexState> {
        self.states.values().filter(|state| predicate(state)).collect()
    }

    /// Démarre une transaction
    pub fn begin_transaction(&mut self) -> TransactionId {
        let transaction_id = TransactionId::new();
        self.transactions.push(StateTransaction::new(transaction_id.clone()));
        transaction_id
    }

    /// Ajoute une opération de création à la transaction courante
    pub fn add_create_operation(&mut self, transaction_id: &TransactionId, id: String) -> Result<(), StateError> {
        if let Some(transaction) = self.transactions.iter_mut().find(|t| t.id == *transaction_id) {
            transaction.add_create(id);
            Ok(())
        } else {
            Err(StateError::TransactionNotFound)
        }
    }

    /// Ajoute une opération de mise à jour à la transaction courante
    pub fn add_update_operation(&mut self, transaction_id: &TransactionId, id: String, updates: HashMap<String, StateValue>) -> Result<(), StateError> {
        if let Some(transaction) = self.transactions.iter_mut().find(|t| t.id == *transaction_id) {
            transaction.add_update(id, updates);
            Ok(())
        } else {
            Err(StateError::TransactionNotFound)
        }
    }

    /// Ajoute une opération de suppression à la transaction courante
    pub fn add_delete_operation(&mut self, transaction_id: &TransactionId, id: String) -> Result<(), StateError> {
        if let Some(transaction) = self.transactions.iter_mut().find(|t| t.id == *transaction_id) {
            transaction.add_delete(id);
            Ok(())
        } else {
            Err(StateError::TransactionNotFound)
        }
    }

    /// Valide une transaction
    pub fn commit_transaction(&mut self, transaction_id: &TransactionId) -> Result<(), StateError> {
        let operations = {
            let transaction = self.transactions.iter()
                .find(|t| t.id == *transaction_id)
                .ok_or(StateError::TransactionNotFound)?;
            transaction.operations.clone()
        };

        // Appliquer les changements
        for operation in operations {
            match operation {
                StateOperation::Create(id) => {
                    self.create_state(id)?;
                }
                StateOperation::Update(id, updates) => {
                    self.update_state(&id, updates)?;
                }
                StateOperation::Delete(id) => {
                    self.delete_state(&id)?;
                }
            }
        }

        // Supprimer la transaction
        self.transactions.retain(|t| t.id != *transaction_id);
        self.take_snapshot("commit_transaction");

        Ok(())
    }

    /// Annule une transaction
    pub fn rollback_transaction(&mut self, transaction_id: &TransactionId) -> Result<(), StateError> {
        self.transactions.retain(|t| t.id != *transaction_id);
        self.take_snapshot("rollback_transaction");
        Ok(())
    }

    /// Prend un snapshot de l'état actuel
    fn take_snapshot(&mut self, operation: &str) {
        if self.history.len() >= self.max_history_size {
            self.history.pop_front();
        }

        let snapshot = StateSnapshot {
            timestamp: Instant::now(),
            operation: operation.to_string(),
            state_count: self.states.len(),
            states: self.states.clone(),
        };

        self.history.push_back(snapshot);
    }

    /// Restaure un état depuis l'historique
    pub fn restore_from_history(&mut self, index: usize) -> Result<(), StateError> {
        if index >= self.history.len() {
            return Err(StateError::InvalidHistoryIndex(index));
        }

        let snapshot = &self.history[index];
        self.states = snapshot.states.clone();

        Ok(())
    }

    /// Obtient les statistiques des états
    pub fn get_stats(&self) -> StateStats {
        let mut property_counts = HashMap::new();
        let mut total_properties = 0;

        for state in self.states.values() {
            for (key, _) in &state.properties {
                *property_counts.entry(key.clone()).or_insert(0) += 1;
            }
            total_properties += state.properties.len();
        }

        // Use history to get last operation, timestamp, state_count
        let last_snapshot = self.history.back();
        let last_operation = last_snapshot.map(|s| s.operation().to_string());
        let last_timestamp = last_snapshot.map(|s| s.timestamp());
        let last_state_count = last_snapshot.map(|s| s.state_count());

        StateStats {
            total_states: self.states.len(),
            total_properties,
            property_counts,
            history_size: self.history.len(),
            active_transactions: self.transactions.len(),
            last_operation,
            last_timestamp,
            last_state_count,
        }
    }

    /// Nettoie les états expirés
    pub fn cleanup_expired_states(&mut self, max_age: Duration) {
        let mut to_remove = Vec::new();

        for (id, state) in &self.states {
            if state.time_since_update() > max_age {
                to_remove.push(id.clone());
            }
        }

        for id in &to_remove {
            self.states.remove(id);
        }

        if !to_remove.is_empty() {
            self.take_snapshot("cleanup_expired");
        }
    }
}

/// Erreurs de gestion d'état
#[derive(Debug, Clone)]
pub enum StateError {
    StateNotFound(String),
    StateAlreadyExists(String),
    TransactionNotFound,
    InvalidHistoryIndex(usize),
    PropertyNotFound(String),
}

impl std::fmt::Display for StateError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            StateError::StateNotFound(id) => write!(f, "State not found: {}", id),
            StateError::StateAlreadyExists(id) => write!(f, "State already exists: {}", id),
            StateError::TransactionNotFound => write!(f, "Transaction not found"),
            StateError::InvalidHistoryIndex(idx) => write!(f, "Invalid history index: {}", idx),
            StateError::PropertyNotFound(key) => write!(f, "Property not found: {}", key),
        }
    }
}

impl std::error::Error for StateError {}

/// ID de transaction
#[derive(Clone, Debug, Eq, PartialEq, Hash)]
pub struct TransactionId(String);

impl TransactionId {
    pub fn new() -> Self {
        TransactionId(format!("tx_{}", Instant::now().elapsed().as_nanos()))
    }
}

/// Transaction d'état
#[derive(Debug)]
struct StateTransaction {
    id: TransactionId,
    operations: Vec<StateOperation>,
}

impl StateTransaction {
    fn new(id: TransactionId) -> Self {
        StateTransaction {
            id,
            operations: Vec::new(),
        }
    }

    /// Add a create operation
    pub fn add_create(&mut self, id: String) {
        self.operations.push(StateOperation::Create(id));
    }

    /// Add an update operation
    pub fn add_update(&mut self, id: String, updates: HashMap<String, StateValue>) {
        self.operations.push(StateOperation::Update(id, updates));
    }

    /// Add a delete operation
    pub fn add_delete(&mut self, id: String) {
        self.operations.push(StateOperation::Delete(id));
    }
}

/// Opération d'état
#[derive(Clone, Debug)]
enum StateOperation {
    Create(String),
    Update(String, HashMap<String, StateValue>),
    Delete(String),
}

/// Snapshot d'état
#[derive(Clone, Debug)]
struct StateSnapshot {
    timestamp: Instant,
    operation: String,
    state_count: usize,
    states: HashMap<String, ComplexState>,
}

impl StateSnapshot {
    /// Get the timestamp
    pub fn timestamp(&self) -> Instant {
        self.timestamp
    }

    /// Get the operation
    pub fn operation(&self) -> &str {
        &self.operation
    }

    /// Get the state count
    pub fn state_count(&self) -> usize {
        self.state_count
    }
}

/// Statistiques des états
#[derive(Debug)]
pub struct StateStats {
    pub total_states: usize,
    pub total_properties: usize,
    pub property_counts: HashMap<String, usize>,
    pub history_size: usize,
    pub active_transactions: usize,
    pub last_operation: Option<String>,
    pub last_timestamp: Option<Instant>,
    pub last_state_count: Option<usize>,
}

/// Gestionnaire d'événements d'état
pub struct StateEventManager {
    listeners: HashMap<String, Vec<Box<dyn Fn(&StateEvent) + Send + Sync>>>,
}

impl StateEventManager {
    pub fn new() -> Self {
        StateEventManager {
            listeners: HashMap::new(),
        }
    }

    /// Enregistre un listener pour un type d'événement
    pub fn add_listener<F>(&mut self, event_type: &str, listener: F)
    where
        F: Fn(&StateEvent) + Send + Sync + 'static,
    {
        self.listeners.entry(event_type.to_string())
            .or_insert_with(Vec::new)
            .push(Box::new(listener));
    }

    /// Émet un événement
    pub fn emit_event(&self, event: StateEvent) {
        if let Some(listeners) = self.listeners.get(&event.event_type) {
            for listener in listeners {
                listener(&event);
            }
        }
    }
}

/// Événement d'état
#[derive(Clone, Debug)]
pub struct StateEvent {
    pub event_type: String,
    pub state_id: String,
    pub timestamp: Instant,
    pub data: HashMap<String, String>,
}

impl StateEvent {
    pub fn new(event_type: String, state_id: String) -> Self {
        StateEvent {
            event_type,
            state_id,
            timestamp: Instant::now(),
            data: HashMap::new(),
        }
    }

    pub fn with_data(mut self, key: String, value: String) -> Self {
        self.data.insert(key, value);
        self
    }
}

/// Builder pour créer des états complexes
pub struct StateBuilder {
    id: String,
    properties: HashMap<String, StateValue>,
    metadata: HashMap<String, String>,
}

impl StateBuilder {
    pub fn new(id: String) -> Self {
        StateBuilder {
            id,
            properties: HashMap::new(),
            metadata: HashMap::new(),
        }
    }

    pub fn property(mut self, key: String, value: StateValue) -> Self {
        self.properties.insert(key, value);
        self
    }

    pub fn metadata(mut self, key: String, value: String) -> Self {
        self.metadata.insert(key, value);
        self
    }

    pub fn build(self) -> ComplexState {
        let mut state = ComplexState::new(self.id);
        state.properties = self.properties;
        state.metadata = self.metadata;
        state
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Arc;

    fn create_debug_writer() -> Arc<Mutex<DebugWriter>> {
        Arc::new(Mutex::new(DebugWriter))
    }

    #[test]
    fn test_state_creation_and_updates() {
        let debug_writer = create_debug_writer();
        let mut manager = StateManager::new(10, debug_writer);

        // Créer un état
        manager.create_state("test_state".to_string()).unwrap();

        // Mettre à jour l'état
        let mut updates = HashMap::new();
        updates.insert("health".to_string(), StateValue::Integer(100));
        updates.insert("name".to_string(), StateValue::String("Test Entity".to_string()));

        manager.update_state("test_state", updates).unwrap();

        // Vérifier l'état
        let state = manager.get_state("test_state").unwrap();
        assert_eq!(state.get_property("health"), Some(&StateValue::Integer(100)));
        assert_eq!(state.get_property("name"), Some(&StateValue::String("Test Entity".to_string())));
    }

    #[test]
    fn test_transaction_management() {
        let debug_writer = create_debug_writer();
        let mut manager = StateManager::new(10, debug_writer);

        let tx_id = manager.begin_transaction();

        // Ajouter des opérations à la transaction
        manager.add_create_operation(&tx_id, "state1".to_string()).unwrap();
        manager.add_create_operation(&tx_id, "state2".to_string()).unwrap();

        // Valider la transaction
        manager.commit_transaction(&tx_id).unwrap();

        // Vérifier que les états existent
        assert!(manager.get_state("state1").is_some());
        assert!(manager.get_state("state2").is_some());
    }

    #[test]
    fn test_state_query() {
        let debug_writer = create_debug_writer();
        let mut manager = StateManager::new(10, debug_writer);

        manager.create_state("player1".to_string()).unwrap();
        manager.create_state("player2".to_string()).unwrap();

        let mut updates = HashMap::new();
        updates.insert("type".to_string(), StateValue::String("player".to_string()));
        manager.update_state("player1", updates.clone()).unwrap();
        manager.update_state("player2", updates).unwrap();

        // Rechercher les états de type "player"
        let players = manager.query_states(&|state| {
            matches!(state.get_property("type"), Some(StateValue::String(s)) if s == "player")
        });

        assert_eq!(players.len(), 2);
    }
}