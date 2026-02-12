// src/modules/deterministic_planning.rs

use std::collections::{HashMap, HashSet, BinaryHeap};
use std::cmp::Reverse;
use crate::utils::debug_writer::DebugWriter;

/// État dans l'espace de recherche
#[derive(Clone, Debug, Eq, PartialEq)]
pub struct PlanningState {
    pub id: String,
    pub variables: HashMap<String, String>,
}

/// Action planifiable
#[derive(Clone, Debug)]
pub struct PlanningAction {
    pub name: String,
    pub preconditions: Vec<Condition>,
    pub effects: Vec<Effect>,
    pub cost: i32,
}

/// Condition pour une action
#[derive(Clone, Debug)]
pub struct Condition {
    pub variable: String,
    pub value: String,
    pub negated: bool,
}

/// Effet d'une action
#[derive(Clone, Debug)]
pub struct Effect {
    pub variable: String,
    pub value: String,
}

/// But à atteindre
#[derive(Clone, Debug)]
pub struct PlanningGoal {
    pub conditions: Vec<Condition>,
}

/// Plan trouvé
#[derive(Clone, Debug)]
pub struct Plan {
    pub actions: Vec<String>,
    pub total_cost: i32,
    pub states_explored: usize,
}

/// Planificateur déterministe utilisant A*
pub struct DeterministicPlanner {
    actions: Vec<PlanningAction>,
}

impl DeterministicPlanner {
    pub fn new() -> Self {
        DeterministicPlanner {
            actions: Vec::new(),
        }
    }

    /// Ajoute une action au planificateur
    pub fn add_action(&mut self, action: PlanningAction) {
        DebugWriter::info(&format!("Action ajoutée: {}", action.name));
        self.actions.push(action);
    }

    /// Trouve un plan pour atteindre le but depuis l'état initial
    pub fn find_plan(&self, initial_state: &PlanningState, goal: &PlanningGoal) -> Option<Plan> {
        let mut frontier = BinaryHeap::new();
        let mut explored = HashSet::new();
        let mut came_from = HashMap::new();
        let mut g_score = HashMap::new();
        let mut state_registry = HashMap::new();

        // État initial
        let initial_id = initial_state.id.clone();
        state_registry.insert(initial_id.clone(), initial_state.clone());
        frontier.push(Reverse((0, initial_id.clone())));
        g_score.insert(initial_id.clone(), 0);

        let mut states_explored = 0;

        while let Some(Reverse((cost, current_id))) = frontier.pop() {
            states_explored += 1;

            if explored.contains(&current_id) {
                continue;
            }
            explored.insert(current_id.clone());

            let current_state = state_registry.get(&current_id).unwrap().clone();

            // Vérifier si le but est atteint
            if self.goal_reached(&current_state, goal) {
                return Some(self.reconstruct_plan(&current_id, &came_from, cost, states_explored));
            }

            // Explorer les actions applicables
            for action in &self.actions {
                if self.action_applicable(&current_state, action) {
                    let new_state = self.apply_action(&current_state, action);
                    let new_state_id = format!("state_{}", state_registry.len());
                    state_registry.insert(new_state_id.clone(), new_state.clone());
                    let new_cost = cost + action.cost;

                    if new_cost < *g_score.get(&new_state_id).unwrap_or(&i32::MAX) {
                        g_score.insert(new_state_id.clone(), new_cost);
                        came_from.insert(new_state_id.clone(), (current_id.clone(), action.name.clone()));
                        frontier.push(Reverse((new_cost, new_state_id)));
                    }
                }
            }
        }

        None // Aucun plan trouvé
    }

    /// Vérifie si une action est applicable dans l'état donné
    fn action_applicable(&self, state: &PlanningState, action: &PlanningAction) -> bool {
        action.preconditions.iter().all(|condition| {
            match state.variables.get(&condition.variable) {
                Some(current_value) => {
                    if condition.negated {
                        current_value != &condition.value
                    } else {
                        current_value == &condition.value
                    }
                }
                None => !condition.negated, // Variable non définie = faux si non négaté
            }
        })
    }

    /// Applique une action à un état
    fn apply_action(&self, state: &PlanningState, action: &PlanningAction) -> PlanningState {
        let mut new_state = state.clone();
        for effect in &action.effects {
            new_state.variables.insert(effect.variable.clone(), effect.value.clone());
        }
        new_state
    }

    /// Vérifie si le but est atteint
    fn goal_reached(&self, state: &PlanningState, goal: &PlanningGoal) -> bool {
        goal.conditions.iter().all(|condition| {
            match state.variables.get(&condition.variable) {
                Some(current_value) => {
                    if condition.negated {
                        current_value != &condition.value
                    } else {
                        current_value == &condition.value
                    }
                }
                None => false,
            }
        })
    }

    /// Reconstruit le plan depuis l'état final
    fn reconstruct_plan(&self, final_state_id: &String, came_from: &HashMap<String, (String, String)>, cost: i32, states_explored: usize) -> Plan {
        let mut actions = Vec::new();
        let mut current_id = final_state_id.clone();

        while let Some((prev_id, action_name)) = came_from.get(&current_id) {
            actions.push(action_name.clone());
            current_id = prev_id.clone();
        }

        actions.reverse();

        Plan {
            actions,
            total_cost: cost,
            states_explored,
        }
    }

    /// Optimise un plan existant
    pub fn optimize_plan(&self, plan: &Plan) -> Plan {
        // Suppression des actions redondantes
        let mut optimized_actions = Vec::new();
        let mut necessary_actions = HashSet::new();

        // Analyse des dépendances (simplifiée)
        for action_name in &plan.actions {
            if let Some(action) = self.actions.iter().find(|a| a.name == *action_name) {
                // Vérifier si l'action est nécessaire pour les effets suivants
                let mut necessary = false;
                for _effect in &action.effects {
                    // TODO: Analyse plus sophistiquée des dépendances
                    if rand::random::<f64>() < 0.7 { // Placeholder
                        necessary = true;
                        break;
                    }
                }
                if necessary {
                    necessary_actions.insert(action_name.clone());
                }
            }
        }

        for action in &plan.actions {
            if necessary_actions.contains(action) {
                optimized_actions.push(action.clone());
            }
        }

        Plan {
            actions: optimized_actions,
            total_cost: plan.total_cost,
            states_explored: plan.states_explored,
        }
    }
}

/// Constructeur d'actions
pub struct ActionBuilder {
    action: PlanningAction,
}

impl ActionBuilder {
    pub fn new(name: String) -> Self {
        ActionBuilder {
            action: PlanningAction {
                name,
                preconditions: Vec::new(),
                effects: Vec::new(),
                cost: 1,
            }
        }
    }

    pub fn precondition(mut self, variable: String, value: String, negated: bool) -> Self {
        self.action.preconditions.push(Condition { variable, value, negated });
        self
    }

    pub fn effect(mut self, variable: String, value: String) -> Self {
        self.action.effects.push(Effect { variable, value });
        self
    }

    pub fn cost(mut self, cost: i32) -> Self {
        self.action.cost = cost;
        self
    }

    pub fn build(self) -> PlanningAction {
        self.action
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_deterministic_planning() {
        let mut planner = DeterministicPlanner::new();

        // Définir des actions
        let move_action = ActionBuilder::new("move".to_string())
            .precondition("location".to_string(), "home".to_string(), false)
            .effect("location".to_string(), "office".to_string())
            .cost(2)
            .build();

        let work_action = ActionBuilder::new("work".to_string())
            .precondition("location".to_string(), "office".to_string(), false)
            .effect("has_worked".to_string(), "true".to_string())
            .cost(1)
            .build();

        planner.add_action(move_action);
        planner.add_action(work_action);

        // État initial et but
        let initial_state = PlanningState {
            id: "initial".to_string(),
            variables: [("location".to_string(), "home".to_string())].iter().cloned().collect(),
        };

        let goal = PlanningGoal {
            conditions: vec![
                Condition {
                    variable: "has_worked".to_string(),
                    value: "true".to_string(),
                    negated: false,
                }
            ],
        };

        // Trouver un plan
        let plan = planner.find_plan(&initial_state, &goal);
        assert!(plan.is_some());

        let plan = plan.unwrap();
        assert!(plan.actions.contains(&"move".to_string()));
        assert!(plan.actions.contains(&"work".to_string()));
        assert!(plan.total_cost > 0);
    }
}