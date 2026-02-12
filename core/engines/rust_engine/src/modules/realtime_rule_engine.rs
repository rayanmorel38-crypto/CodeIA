// src/modules/realtime_rule_engine.rs

use std::collections::{HashMap, VecDeque};
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use crate::utils::debug_writer::DebugWriter;

/// Règle temps réel
#[derive(Clone, Debug)]
pub struct RealtimeRule {
    pub id: String,
    pub name: String,
    pub conditions: Vec<Condition>,
    pub actions: Vec<RuleAction>,
    pub priority: i32,
    pub cooldown: Duration,
    pub last_triggered: Option<Instant>,
}

/// Condition pour une règle
#[derive(Clone, Debug)]
pub enum Condition {
    VariableEquals { variable: String, value: String },
    VariableGreater { variable: String, value: f64 },
    VariableLess { variable: String, value: f64 },
    TimeElapsed { duration: Duration },
    EventOccurred { event_type: String, count: usize },
}

/// Action exécutée par une règle
#[derive(Clone, Debug)]
pub enum RuleAction {
    SetVariable { variable: String, value: String },
    TriggerEvent { event_type: String, data: HashMap<String, String> },
    ExecuteFunction { function_name: String, parameters: Vec<String> },
    LogMessage { level: String, message: String },
}

/// Événement dans le système
#[derive(Clone, Debug)]
pub struct SystemEvent {
    pub event_type: String,
    pub timestamp: Instant,
    pub data: HashMap<String, String>,
}

/// Moteur de règles temps réel
pub struct RealtimeRuleEngine {
    rules: Arc<Mutex<Vec<RealtimeRule>>>,
    facts: Arc<Mutex<HashMap<String, FactValue>>>,
    event_queue: Arc<Mutex<VecDeque<SystemEvent>>>,
    event_history: Arc<Mutex<HashMap<String, Vec<Instant>>>>,
    is_running: Arc<Mutex<bool>>,
}

#[derive(Clone, Debug)]
pub enum FactValue {
    String(String),
    Number(f64),
    Boolean(bool),
}

impl RealtimeRuleEngine {
    pub fn new() -> Self {
        RealtimeRuleEngine {
            rules: Arc::new(Mutex::new(Vec::new())),
            facts: Arc::new(Mutex::new(HashMap::new())),
            event_queue: Arc::new(Mutex::new(VecDeque::new())),
            event_history: Arc::new(Mutex::new(HashMap::new())),
            is_running: Arc::new(Mutex::new(false)),
        }
    }

    /// Ajoute une règle au moteur
    pub fn add_rule(&self, rule: RealtimeRule) {
        let rule_clone = rule.clone();
        let mut rules = self.rules.lock().unwrap();
        rules.push(rule);
        // Trier par priorité (plus haute en premier)
        rules.sort_by(|a, b| b.priority.cmp(&a.priority));
        DebugWriter::info(&format!("Règle ajoutée: {} (priorité: {})", rule_clone.name, rule_clone.priority));
    }

    /// Met à jour un fait
    pub fn update_fact(&self, key: String, value: FactValue) {
        let mut facts = self.facts.lock().unwrap();
        facts.insert(key, value);
    }

    /// Ajoute un événement à la queue
    pub fn queue_event(&self, event: SystemEvent) {
        let event_clone = event.clone();
        let mut queue = self.event_queue.lock().unwrap();
        queue.push_back(event);

        // Mettre à jour l'historique
        let mut history = self.event_history.lock().unwrap();
        history.entry(event_clone.event_type.clone())
            .or_insert_with(Vec::new)
            .push(event_clone.timestamp);
    }

    /// Traite les événements et applique les règles
    pub fn process_events(&self) -> Vec<String> {
        // Mark engine as running during processing to avoid unused field warning
        {
            let mut running = self.is_running.lock().unwrap();
            *running = true;
        }

        let mut triggered_rules = Vec::new();
        let mut queue = self.event_queue.lock().unwrap();

        while let Some(event) = queue.pop_front() {
            let rules_to_check = {
                let rules = self.rules.lock().unwrap();
                rules.clone()
            };

            for rule in rules_to_check {
                if self.rule_matches(&rule, &event) {
                    if self.can_trigger_rule(&rule) {
                        self.execute_rule(&rule, &event);
                        triggered_rules.push(format!("Règle '{}' déclenchée", rule.name));

                        // Marquer la règle comme déclenchée
                        let mut rules = self.rules.lock().unwrap();
                        if let Some(rule_mut) = rules.iter_mut().find(|r| r.id == rule.id) {
                            rule_mut.last_triggered = Some(Instant::now());
                        }
                    }
                }
            }
        }

        // Processing finished
        {
            let mut running = self.is_running.lock().unwrap();
            *running = false;
        }

        triggered_rules
    }

    /// Vérifie si une règle correspond à un événement
    fn rule_matches(&self, rule: &RealtimeRule, _event: &SystemEvent) -> bool {
        let facts = self.facts.lock().unwrap();
        let history = self.event_history.lock().unwrap();

        for condition in &rule.conditions {
            match condition {
                Condition::VariableEquals { variable, value } => {
                    if let Some(FactValue::String(fact_value)) = facts.get(variable) {
                        if fact_value != value {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
                Condition::VariableGreater { variable, value } => {
                    if let Some(FactValue::Number(fact_value)) = facts.get(variable) {
                        if fact_value <= value {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
                Condition::VariableLess { variable, value } => {
                    if let Some(FactValue::Number(fact_value)) = facts.get(variable) {
                        if fact_value >= value {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
                Condition::TimeElapsed { duration } => {
                    if let Some(last_triggered) = rule.last_triggered {
                        if last_triggered.elapsed() < *duration {
                            return false;
                        }
                    }
                }
                Condition::EventOccurred { event_type, count } => {
                    if let Some(event_times) = history.get(event_type) {
                        // Compter les événements récents (dernière minute)
                        let recent_count = event_times.iter()
                            .filter(|&&time| time.elapsed() < Duration::from_secs(60))
                            .count();
                        if recent_count < *count {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            }
        }

        true
    }

    /// Vérifie si une règle peut être déclenchée (cooldown)
    fn can_trigger_rule(&self, rule: &RealtimeRule) -> bool {
        if let Some(last_triggered) = rule.last_triggered {
            last_triggered.elapsed() >= rule.cooldown
        } else {
            true // Première fois
        }
    }

    /// Exécute les actions d'une règle
    fn execute_rule(&self, rule: &RealtimeRule, _event: &SystemEvent) {
        for action in &rule.actions {
            match action {
                RuleAction::SetVariable { variable, value } => {
                    self.update_fact(variable.clone(), FactValue::String(value.clone()));
                    DebugWriter::info(&format!("Variable '{}' définie à '{}'", variable, value));
                }
                RuleAction::TriggerEvent { event_type, data } => {
                    let new_event = SystemEvent {
                        event_type: event_type.clone(),
                        timestamp: Instant::now(),
                        data: data.clone(),
                    };
                    self.queue_event(new_event);
                    DebugWriter::info(&format!("Événement '{}' déclenché", event_type));
                }
                RuleAction::ExecuteFunction { function_name, parameters } => {
                    DebugWriter::info(&format!("Fonction '{}' exécutée avec paramètres: {:?}",
                                              function_name, parameters));
                    // TODO: Implémenter l'exécution de fonctions dynamiques
                }
                RuleAction::LogMessage { level, message } => {
                    let formatted_message = format!("[{}] {}", level.to_uppercase(), message);
                    match level.as_str() {
                        "error" => DebugWriter::error(&formatted_message),
                        "warn" => DebugWriter::info(&format!("WARN: {}", formatted_message)),
                        _ => DebugWriter::info(&formatted_message),
                    }
                }
            }
        }
    }

    /// Obtient les statistiques du moteur
    pub fn get_stats(&self) -> RuleEngineStats {
        let rules = self.rules.lock().unwrap();
        let facts = self.facts.lock().unwrap();
        let queue = self.event_queue.lock().unwrap();
        let history = self.event_history.lock().unwrap();

        RuleEngineStats {
            rule_count: rules.len(),
            fact_count: facts.len(),
            queued_events: queue.len(),
            total_events_processed: history.values().map(|v| v.len()).sum(),
        }
    }
}

/// Statistiques du moteur de règles
#[derive(Debug)]
pub struct RuleEngineStats {
    pub rule_count: usize,
    pub fact_count: usize,
    pub queued_events: usize,
    pub total_events_processed: usize,
}

/// Constructeur de règles
pub struct RuleBuilder {
    rule: RealtimeRule,
}

impl RuleBuilder {
    pub fn new(id: String, name: String) -> Self {
        RuleBuilder {
            rule: RealtimeRule {
                id,
                name,
                conditions: Vec::new(),
                actions: Vec::new(),
                priority: 0,
                cooldown: Duration::from_millis(100),
                last_triggered: None,
            }
        }
    }

    pub fn condition(mut self, condition: Condition) -> Self {
        self.rule.conditions.push(condition);
        self
    }

    pub fn action(mut self, action: RuleAction) -> Self {
        self.rule.actions.push(action);
        self
    }

    pub fn priority(mut self, priority: i32) -> Self {
        self.rule.priority = priority;
        self
    }

    pub fn cooldown(mut self, cooldown: Duration) -> Self {
        self.rule.cooldown = cooldown;
        self
    }

    pub fn build(self) -> RealtimeRule {
        self.rule
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_realtime_rule_engine() {
        let engine = RealtimeRuleEngine::new();

        // Créer une règle simple
        let rule = RuleBuilder::new("test_rule".to_string(), "Test Rule".to_string())
            .condition(Condition::VariableEquals {
                variable: "status".to_string(),
                value: "active".to_string(),
            })
            .action(RuleAction::SetVariable {
                variable: "response".to_string(),
                value: "triggered".to_string(),
            })
            .priority(10)
            .build();

        engine.add_rule(rule);

        // Définir le fait déclencheur
        engine.update_fact("status".to_string(), FactValue::String("active".to_string()));

        // Créer un événement
        let event = SystemEvent {
            event_type: "status_change".to_string(),
            timestamp: Instant::now(),
            data: HashMap::new(),
        };

        engine.queue_event(event);

        // Traiter les événements
        let triggered = engine.process_events();
        assert_eq!(triggered.len(), 1);
        assert!(triggered[0].contains("Test Rule"));

        // Vérifier que la variable a été définie
        // Note: Dans un vrai test, on vérifierait les facts
    }
}