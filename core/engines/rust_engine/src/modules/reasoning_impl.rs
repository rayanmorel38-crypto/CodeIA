/// Implémentation avancée du raisonnement logique
use std::collections::{HashMap, HashSet};
use serde::{Serialize, Deserialize};

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq, Eq, Hash)]
pub enum LogicalValue {
    True,
    False,
    Unknown,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct LogicalProposition {
    pub id: String,
    pub statement: String,
    pub value: LogicalValue,
    pub confidence: f64, // 0.0 à 1.0
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub enum LogicalOperator {
    And,
    Or,
    Not,
    Implies, // →
    IfAndOnlyIf, // ↔
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct LogicalRule {
    pub premises: Vec<String>, // IDs de propositions
    pub conclusion: String,    // ID de proposition
    pub operator: LogicalOperator,
    pub weight: f64, // Force de la règle
}

pub struct ReasoningEngine {
    propositions: HashMap<String, LogicalProposition>,
    rules: Vec<LogicalRule>,
    inference_cache: HashMap<String, LogicalValue>,
    visited_during_inference: HashSet<String>,
}

impl ReasoningEngine {
    pub fn new() -> Self {
        ReasoningEngine {
            propositions: HashMap::new(),
            rules: Vec::new(),
            inference_cache: HashMap::new(),
            visited_during_inference: HashSet::new(),
        }
    }

    /// Ajoute une proposition de base
    pub fn add_proposition(&mut self, id: String, statement: String, initial_value: LogicalValue) {
        let prop = LogicalProposition {
            id: id.clone(),
            statement,
            value: initial_value,
            confidence: 1.0,
        };
        self.propositions.insert(id, prop);
    }

    /// Ajoute une règle logique
    pub fn add_rule(&mut self, rule: LogicalRule) {
        self.rules.push(rule);
    }

    /// Infère la valeur d'une proposition en utilisant les règles
    pub fn infer(&mut self, prop_id: &str) -> LogicalValue {
        if let Some(cached) = self.inference_cache.get(prop_id) {
            return cached.clone();
        }

        // Éviter les boucles infinies
        if self.visited_during_inference.contains(prop_id) {
            return LogicalValue::Unknown;
        }
        self.visited_during_inference.insert(prop_id.to_string());

        // Si la proposition est définie, retourner sa valeur
        if let Some(prop) = self.propositions.get(prop_id) {
            let result = prop.value.clone();
            self.inference_cache.insert(prop_id.to_string(), result.clone());
            return result;
        }

        // Appliquer les règles
        let mut inferred_value = LogicalValue::Unknown;
        let mut max_confidence = 0.0;

        for rule in self.rules.clone() {
            if rule.conclusion != prop_id {
                continue;
            }

            let premise_values: Vec<_> = rule
                .premises
                .iter()
                .map(|p| self.infer(p))
                .collect();

            let rule_result = match rule.operator {
                LogicalOperator::And => {
                    if premise_values.iter().all(|v| v == &LogicalValue::True) {
                        LogicalValue::True
                    } else if premise_values.iter().any(|v| v == &LogicalValue::False) {
                        LogicalValue::False
                    } else {
                        LogicalValue::Unknown
                    }
                }
                LogicalOperator::Or => {
                    if premise_values.iter().any(|v| v == &LogicalValue::True) {
                        LogicalValue::True
                    } else if premise_values.iter().all(|v| v == &LogicalValue::False) {
                        LogicalValue::False
                    } else {
                        LogicalValue::Unknown
                    }
                }
                LogicalOperator::Not => {
                    if premise_values.len() == 1 {
                        match &premise_values[0] {
                            LogicalValue::True => LogicalValue::False,
                            LogicalValue::False => LogicalValue::True,
                            LogicalValue::Unknown => LogicalValue::Unknown,
                        }
                    } else {
                        LogicalValue::Unknown
                    }
                }
                LogicalOperator::Implies => {
                    if premise_values.len() >= 2 {
                        match (&premise_values[0], &premise_values[1]) {
                            (LogicalValue::True, LogicalValue::False) => LogicalValue::False,
                            _ => LogicalValue::True,
                        }
                    } else {
                        LogicalValue::Unknown
                    }
                }
                LogicalOperator::IfAndOnlyIf => {
                    if premise_values.len() >= 2 {
                        if premise_values[0] == premise_values[1] {
                            LogicalValue::True
                        } else {
                            LogicalValue::False
                        }
                    } else {
                        LogicalValue::Unknown
                    }
                }
            };

            // Mettre à jour si le résultat est plus confiant
            if rule.weight > max_confidence {
                inferred_value = rule_result;
                max_confidence = rule.weight;
            }
        }

        self.inference_cache
            .insert(prop_id.to_string(), inferred_value.clone());
        inferred_value
    }

    /// Effectue une inférence complète (forward chaining)
    pub fn forward_chain(&mut self) -> HashMap<String, LogicalValue> {
        let mut results = HashMap::new();
        let prop_ids: Vec<String> = self.propositions.keys().cloned().collect();

        for prop_id in prop_ids {
            let value = self.infer(&prop_id);
            results.insert(prop_id, value);
        }

        results
    }

    /// Obtient toutes les propositions probables
    pub fn get_probable_conclusions(&self) -> Vec<(String, LogicalValue, f64)> {
        let mut conclusions = Vec::new();

        for (prop_id, prop) in &self.propositions {
            if prop.value != LogicalValue::Unknown {
                conclusions.push((prop_id.clone(), prop.value.clone(), prop.confidence));
            }
        }

        conclusions.sort_by(|a, b| b.2.partial_cmp(&a.2).unwrap_or(std::cmp::Ordering::Equal));
        conclusions
    }
}

impl Default for ReasoningEngine {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_logic() {
        let mut engine = ReasoningEngine::new();
        engine.add_proposition("A".to_string(), "A is true".to_string(), LogicalValue::True);
        engine.add_proposition("B".to_string(), "B is false".to_string(), LogicalValue::False);

        assert_eq!(engine.infer("A"), LogicalValue::True);
        assert_eq!(engine.infer("B"), LogicalValue::False);
    }

    #[test]
    fn test_and_rule() {
        let mut engine = ReasoningEngine::new();
        engine.add_proposition("P1".to_string(), "P1".to_string(), LogicalValue::True);
        engine.add_proposition("P2".to_string(), "P2".to_string(), LogicalValue::True);

        let rule = LogicalRule {
            premises: vec!["P1".to_string(), "P2".to_string()],
            conclusion: "Result".to_string(),
            operator: LogicalOperator::And,
            weight: 1.0,
        };
        engine.add_rule(rule);

        assert_eq!(engine.infer("Result"), LogicalValue::True);
    }

    #[test]
    fn test_forward_chaining() {
        let mut engine = ReasoningEngine::new();
        engine.add_proposition("A".to_string(), "A".to_string(), LogicalValue::True);
        engine.add_proposition("B".to_string(), "B".to_string(), LogicalValue::True);

        let rule = LogicalRule {
            premises: vec!["A".to_string(), "B".to_string()],
            conclusion: "C".to_string(),
            operator: LogicalOperator::And,
            weight: 1.0,
        };
        engine.add_rule(rule);

        let results = engine.forward_chain();
        assert_eq!(results.get("C"), Some(&LogicalValue::True));
    }
}
