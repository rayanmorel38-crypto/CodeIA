// src/modules/fsm_behavior_trees.rs

use std::collections::{HashMap, HashSet};
use std::sync::{Arc, Mutex};

/// État d'une FSM
#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum FSMState {
    Idle,
    Active,
    Processing,
    Waiting,
    Error,
    Custom(String),
}

/// Transition dans une FSM
pub struct FSMTransition {
    pub from_state: FSMState,
    pub to_state: FSMState,
    pub condition: Box<dyn Fn(&FSMContext) -> bool + Send + Sync>,
    pub action: Option<Box<dyn Fn(&mut FSMContext) + Send + Sync>>,
}

/// Contexte d'une FSM
#[derive(Clone, Debug)]
pub struct FSMContext {
    pub current_state: FSMState,
    pub variables: HashMap<String, String>,
    pub timers: HashMap<String, std::time::Instant>,
}

/// Machine à états finis
pub struct FiniteStateMachine {
    states: HashSet<FSMState>,
    transitions: Vec<FSMTransition>,
    context: Arc<Mutex<FSMContext>>,
    initial_state: FSMState,
}

impl FiniteStateMachine {
    pub fn new(initial_state: FSMState) -> Self {
        let context = FSMContext {
            current_state: initial_state.clone(),
            variables: HashMap::new(),
            timers: HashMap::new(),
        };

        FiniteStateMachine {
            states: HashSet::new(),
            transitions: Vec::new(),
            context: Arc::new(Mutex::new(context)),
            initial_state,
        }
    }

    /// Ajoute un état
    pub fn add_state(&mut self, state: FSMState) {
        self.states.insert(state);
    }

    /// Ajoute une transition
    pub fn add_transition(&mut self, transition: FSMTransition) {
        self.transitions.push(transition);
    }

    /// Met à jour la FSM
    pub fn update(&self) -> Vec<String> {
        let mut actions_performed = Vec::new();
        let mut context = self.context.lock().unwrap();

        for transition in &self.transitions {
            if context.current_state == transition.from_state {
                if (transition.condition)(&context) {
                    // Exécuter l'action si elle existe
                    if let Some(action) = &transition.action {
                        action(&mut context);
                        actions_performed.push(format!("Transition: {:?} -> {:?}",
                                                     transition.from_state, transition.to_state));
                    }

                    // Changer d'état
                    context.current_state = transition.to_state.clone();
                    break;
                }
            }
        }

        actions_performed
    }

    /// Obtient l'état actuel
    pub fn get_current_state(&self) -> FSMState {
        self.context.lock().unwrap().current_state.clone()
    }

    /// Obtient l'état initial
    pub fn get_initial_state(&self) -> &FSMState {
        &self.initial_state
    }

    /// Remet à l'état initial
    pub fn reset_to_initial(&self) {
        let mut context = self.context.lock().unwrap();
        context.current_state = self.initial_state.clone();
        context.variables.clear();
        context.timers.clear();
    }

    /// Définit une variable
    pub fn set_variable(&self, key: String, value: String) {
        let mut context = self.context.lock().unwrap();
        context.variables.insert(key, value);
    }
}

/// Nœud d'un Behavior Tree
pub enum BTNode {
    Sequence(Vec<BTNode>),
    Selector(Vec<BTNode>),
    Decorator(Box<BTNode>, DecoratorType),
    Leaf(Box<dyn Fn(&BTContext) -> BTStatus + Send + Sync>),
}

/// Type de décorateur
#[derive(Clone, Debug)]
pub enum DecoratorType {
    Inverter,
    Succeeder,
    Failer,
    Repeat(u32),
}

/// Statut d'exécution d'un nœud BT
#[derive(Clone, Debug, PartialEq)]
pub enum BTStatus {
    Success,
    Failure,
    Running,
}

/// Contexte d'un Behavior Tree
#[derive(Clone, Debug)]
pub struct BTContext {
    pub variables: HashMap<String, String>,
    pub timers: HashMap<String, std::time::Instant>,
}

/// Behavior Tree
pub struct BehaviorTree {
    root: BTNode,
    context: Arc<Mutex<BTContext>>,
}

impl BehaviorTree {
    pub fn new(root: BTNode) -> Self {
        let context = BTContext {
            variables: HashMap::new(),
            timers: HashMap::new(),
        };

        BehaviorTree {
            root,
            context: Arc::new(Mutex::new(context)),
        }
    }

    /// Exécute le Behavior Tree
    pub fn tick(&self) -> BTStatus {
        let context = self.context.lock().unwrap();
        self.execute_node(&self.root, &context)
    }

    /// Exécute un nœud récursivement
    fn execute_node(&self, node: &BTNode, context: &BTContext) -> BTStatus {
        match node {
            BTNode::Sequence(children) => {
                for child in children {
                    match self.execute_node(child, context) {
                        BTStatus::Failure => return BTStatus::Failure,
                        BTStatus::Running => return BTStatus::Running,
                        BTStatus::Success => continue,
                    }
                }
                BTStatus::Success
            }
            BTNode::Selector(children) => {
                for child in children {
                    match self.execute_node(child, context) {
                        BTStatus::Success => return BTStatus::Success,
                        BTStatus::Running => return BTStatus::Running,
                        BTStatus::Failure => continue,
                    }
                }
                BTStatus::Failure
            }
            BTNode::Decorator(child, decorator_type) => {
                let child_status = self.execute_node(child, context);
                match decorator_type {
                    DecoratorType::Inverter => match child_status {
                        BTStatus::Success => BTStatus::Failure,
                        BTStatus::Failure => BTStatus::Success,
                        BTStatus::Running => BTStatus::Running,
                    },
                    DecoratorType::Succeeder => BTStatus::Success,
                    DecoratorType::Failer => BTStatus::Failure,
                    DecoratorType::Repeat(_count) => {
                        // Répétition non implémentée : renvoyer le statut de l'enfant
                        child_status
                    }
                }
            }
            BTNode::Leaf(action) => action(context),
        }
    }

    /// Définit une variable dans le contexte
    pub fn set_variable(&self, key: String, value: String) {
        let mut context = self.context.lock().unwrap();
        context.variables.insert(key, value);
    }
}

/// Constructeur de Behavior Tree
pub struct BTBuilder {
    nodes: Vec<BTNode>,
}

impl BTBuilder {
    pub fn new() -> Self {
        BTBuilder { nodes: Vec::new() }
    }

    /// Ajoute un nœud Sequence
    pub fn sequence(mut self, children: Vec<BTNode>) -> Self {
        self.nodes.push(BTNode::Sequence(children));
        self
    }

    /// Ajoute un nœud Selector
    pub fn selector(mut self, children: Vec<BTNode>) -> Self {
        self.nodes.push(BTNode::Selector(children));
        self
    }

    /// Ajoute un nœud Leaf
    pub fn leaf<F>(mut self, action: F) -> Self
    where
        F: Fn(&BTContext) -> BTStatus + Send + Sync + 'static,
    {
        self.nodes.push(BTNode::Leaf(Box::new(action)));
        self
    }

    /// Construit le Behavior Tree
    pub fn build(self) -> Option<BTNode> {
        self.nodes.into_iter().next()
    }
}

/// Gestionnaire combiné FSM + Behavior Trees
pub struct StateBehaviorManager {
    fsm: FiniteStateMachine,
    behavior_trees: HashMap<FSMState, BehaviorTree>,
}

impl StateBehaviorManager {
    pub fn new(initial_state: FSMState) -> Self {
        StateBehaviorManager {
            fsm: FiniteStateMachine::new(initial_state),
            behavior_trees: HashMap::new(),
        }
    }

    /// Associe un Behavior Tree à un état FSM
    pub fn add_behavior_tree(&mut self, state: FSMState, bt: BehaviorTree) {
        self.behavior_trees.insert(state, bt);
    }

    /// Met à jour le système complet
    pub fn update(&self) -> Vec<String> {
        let mut actions = Vec::new();

        // Mettre à jour la FSM
        let fsm_actions = self.fsm.update();
        actions.extend(fsm_actions);

        // Exécuter le Behavior Tree de l'état actuel
        let current_state = self.fsm.get_current_state();
        if let Some(bt) = self.behavior_trees.get(&current_state) {
            let bt_status = bt.tick();
            actions.push(format!("BT Status for {:?}: {:?}", current_state, bt_status));
        }

        actions
    }

    /// Définit une variable (propagée à la FSM et aux BT)
    pub fn set_variable(&self, key: String, value: String) {
        self.fsm.set_variable(key.clone(), value.clone());

        for bt in self.behavior_trees.values() {
            bt.set_variable(key.clone(), value.clone());
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_finite_state_machine() {
        let mut fsm = FiniteStateMachine::new(FSMState::Idle);

        // Ajouter des états
        fsm.add_state(FSMState::Idle);
        fsm.add_state(FSMState::Active);

        // Ajouter une transition
        let transition = FSMTransition {
            from_state: FSMState::Idle,
            to_state: FSMState::Active,
            condition: Box::new(|_| true), // Toujours vraie
            action: Some(Box::new(|_| {})),
        };

        fsm.add_transition(transition);

        // Vérifier l'état initial
        assert_eq!(fsm.get_current_state(), FSMState::Idle);

        // Mettre à jour (devrait changer d'état)
        let actions = fsm.update();
        assert!(!actions.is_empty());
        assert_eq!(fsm.get_current_state(), FSMState::Active);
    }

    #[test]
    fn test_behavior_tree() {
        // Créer un BT simple : Sequence avec deux actions qui réussissent
        let bt = BehaviorTree::new(BTNode::Sequence(vec![
            BTNode::Leaf(Box::new(|_| BTStatus::Success)),
            BTNode::Leaf(Box::new(|_| BTStatus::Success)),
        ]));

        let status = bt.tick();
        assert_eq!(status, BTStatus::Success);
    }
}