// src/modules/world_model.rs

use std::collections::{HashMap, HashSet, VecDeque};
use serde::{Serialize, Deserialize};
use crate::utils::debug_writer::DebugWriter;

/// Représentation logique d'une entité dans le monde
#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct WorldEntity {
    pub id: String,
    pub entity_type: String,
    pub properties: HashMap<String, PropertyValue>,
    pub relations: HashMap<String, Vec<String>>, // relation_type -> [entity_ids]
}

/// Valeur de propriété (peut être différente types)
#[derive(Clone, Debug, PartialEq, Serialize, Deserialize)]
pub enum PropertyValue {
    String(String),
    Integer(i64),
    Float(f64),
    Boolean(bool),
    List(Vec<PropertyValue>),
}

/// Relation entre entités
#[derive(Clone, Debug)]
pub struct Relation {
    pub from_id: String,
    pub to_id: String,
    pub relation_type: String,
    pub properties: HashMap<String, PropertyValue>,
}

/// Modèle logique du monde
pub struct WorldModel {
    entities: HashMap<String, WorldEntity>,
    relations: Vec<Relation>,
    constraints: Vec<WorldConstraint>,
    inference_rules: Vec<InferenceRule>,
}

#[derive(Clone, Debug)]
pub struct WorldConstraint {
    pub name: String,
    pub condition: String, // Expression logique
    pub description: String,
}

#[derive(Clone, Debug)]
pub struct InferenceRule {
    pub name: String,
    pub premises: Vec<String>, // Conditions
    pub conclusion: String,    // Conséquence
    pub confidence: f64,
}

impl WorldModel {
    pub fn new() -> Self {
        WorldModel {
            entities: HashMap::new(),
            relations: Vec::new(),
            constraints: Vec::new(),
            inference_rules: Vec::new(),
        }
    }

    /// Ajoute une entité au modèle
    pub fn add_entity(&mut self, entity: WorldEntity) {
        DebugWriter::info(&format!("Ajout entité: {} ({})", entity.id, entity.entity_type));
        self.entities.insert(entity.id.clone(), entity);
    }

    /// Ajoute une relation entre entités
    pub fn add_relation(&mut self, relation: Relation) {
        // Vérifier que les entités existent
        if !self.entities.contains_key(&relation.from_id) ||
           !self.entities.contains_key(&relation.to_id) {
            DebugWriter::error(&format!("Relation invalide: entité(s) inexistante(s)"));
            return;
        }

        // Ajouter la relation aux entités
        if let Some(from_entity) = self.entities.get_mut(&relation.from_id) {
            from_entity.relations
                .entry(relation.relation_type.clone())
                .or_insert_with(Vec::new)
                .push(relation.to_id.clone());
        }

        self.relations.push(relation);
        DebugWriter::info("Relation ajoutée");
    }

    /// Ajoute une contrainte logique
    pub fn add_constraint(&mut self, constraint: WorldConstraint) {
        self.constraints.push(constraint);
        DebugWriter::info("Contrainte ajoutée");
    }

    /// Ajoute une règle d'inférence
    pub fn add_inference_rule(&mut self, rule: InferenceRule) {
        self.inference_rules.push(rule);
        DebugWriter::info("Règle d'inférence ajoutée");
    }

    /// Vérifie la cohérence du modèle
    pub fn check_consistency(&self) -> Vec<String> {
        let mut issues = Vec::new();

        // Vérifier les contraintes
        for constraint in &self.constraints {
            if !self.evaluate_condition(&constraint.condition) {
                issues.push(format!("Contrainte '{}' violée: {}",
                                  constraint.name, constraint.description));
            }
        }

        // Vérifier l'intégrité des relations
        for relation in &self.relations {
            if !self.entities.contains_key(&relation.from_id) {
                issues.push(format!("Relation cassée: {} n'existe pas", relation.from_id));
            }
            if !self.entities.contains_key(&relation.to_id) {
                issues.push(format!("Relation cassée: {} n'existe pas", relation.to_id));
            }
        }

        issues
    }

    /// Applique les règles d'inférence
    pub fn apply_inference(&mut self) -> Vec<String> {
        let mut new_facts = Vec::new();
        let mut conclusions_to_apply = Vec::new();

        // Collecter d'abord toutes les conclusions à appliquer
        for rule in &self.inference_rules {
            if self.evaluate_premises(&rule.premises) {
                conclusions_to_apply.push((rule.name.clone(), rule.conclusion.clone()));
            }
        }

        // Appliquer les conclusions
        for (rule_name, conclusion) in conclusions_to_apply {
            if let Some(new_fact) = self.apply_conclusion(&conclusion) {
                new_facts.push(format!("Inférence '{}': {}", rule_name, new_fact));
            }
        }

        new_facts
    }

    /// Recherche d'entités par propriétés
    pub fn query_entities(&self, property: &str, value: &PropertyValue) -> Vec<&WorldEntity> {
        self.entities.values()
            .filter(|entity| {
                entity.properties.get(property)
                    .map(|prop_val| prop_val == value)
                    .unwrap_or(false)
            })
            .collect()
    }

    /// Recherche de chemins entre entités
    pub fn find_path(&self, from_id: &str, to_id: &str, max_depth: usize) -> Option<Vec<String>> {
        if !self.entities.contains_key(from_id) || !self.entities.contains_key(to_id) {
            return None;
        }

        let mut visited = HashSet::new();
        let mut queue: VecDeque<(String, usize)> = VecDeque::new();
        let mut parent_map: HashMap<String, String> = HashMap::new();

        queue.push_back((from_id.to_string(), 0));
        visited.insert(from_id.to_string());

        while let Some((current, depth)) = queue.pop_front() {
            if current == to_id {
                // Reconstruire le chemin
                let mut path = Vec::new();
                let mut node = current;
                while let Some(parent) = parent_map.get(&node) {
                    path.push(node);
                    node = parent.clone();
                }
                path.push(from_id.to_string());
                path.reverse();
                return Some(path);
            }

            if let Some(entity) = self.entities.get(&current) {
                for (_, related_ids) in &entity.relations {
                    for related_id in related_ids {
                        if !visited.contains(related_id) && depth < max_depth {
                            visited.insert(related_id.clone());
                            parent_map.insert(related_id.clone(), current.clone());
                            queue.push_back((related_id.clone(), depth + 1));
                        }
                    }
                }
            }
        }

        None
    }

    // Méthodes privées d'aide
    fn evaluate_condition(&self, _condition: &str) -> bool {
        // TODO: Implémenter un vrai évaluateur d'expressions logiques
        true // Placeholder
    }

    fn evaluate_premises(&self, _premises: &[String]) -> bool {
        // TODO: Implémenter l'évaluation des prémisses
        rand::random::<f64>() < 0.3 // Placeholder aléatoire
    }

    fn apply_conclusion(&mut self, _conclusion: &str) -> Option<String> {
        // TODO: Implémenter l'application des conclusions
        Some("Nouvelle connaissance déduite".to_string()) // Placeholder
    }
}

/// Constructeur d'entités
pub struct WorldEntityBuilder {
    entity: WorldEntity,
}

impl WorldEntityBuilder {
    pub fn new(id: String, entity_type: String) -> Self {
        WorldEntityBuilder {
            entity: WorldEntity {
                id,
                entity_type,
                properties: HashMap::new(),
                relations: HashMap::new(),
            }
        }
    }

    pub fn with_property(mut self, key: String, value: PropertyValue) -> Self {
        self.entity.properties.insert(key, value);
        self
    }

    pub fn build(self) -> WorldEntity {
        self.entity
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_world_model() {
        let mut world = WorldModel::new();

        // Ajouter des entités
        let agent = WorldEntityBuilder::new("agent1".to_string(), "AI_Agent".to_string())
            .with_property("intelligence".to_string(), PropertyValue::Integer(85))
            .build();

        let task = WorldEntityBuilder::new("task1".to_string(), "Task".to_string())
            .with_property("priority".to_string(), PropertyValue::String("high".to_string()))
            .build();

        world.add_entity(agent);
        world.add_entity(task);

        // Ajouter une relation
        let relation = Relation {
            from_id: "agent1".to_string(),
            to_id: "task1".to_string(),
            relation_type: "assigned_to".to_string(),
            properties: HashMap::new(),
        };
        world.add_relation(relation);

        // Vérifier la cohérence
        let issues = world.check_consistency();
        assert!(issues.is_empty());

        // Tester la recherche
        let high_priority_tasks = world.query_entities("priority",
            &PropertyValue::String("high".to_string()));
        assert_eq!(high_priority_tasks.len(), 1);
    }
}