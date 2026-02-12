/// Self-Learning - Apprentissage autonome par expérience
use std::collections::{HashMap, VecDeque};
use serde::{Serialize, Deserialize};

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Experience {
    pub state: String,
    pub action: String,
    pub reward: f64,
    pub next_state: String,
    pub timestamp: u64,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct QValue {
    pub state: String,
    pub action: String,
    pub q: f64,
    pub visit_count: u32,
}

pub struct QLearningAgent {
    pub q_table: HashMap<(String, String), f64>,
    pub state_visits: HashMap<String, u32>,
    pub action_visits: HashMap<(String, String), u32>,
    pub learning_rate: f64,
    pub discount_factor: f64,
    pub epsilon: f64,
    pub experiences: VecDeque<Experience>,
    pub max_experiences: usize,
}

impl QLearningAgent {
    pub fn new(learning_rate: f64, discount_factor: f64, epsilon: f64) -> Self {
        QLearningAgent {
            q_table: HashMap::new(),
            state_visits: HashMap::new(),
            action_visits: HashMap::new(),
            learning_rate,
            discount_factor,
            epsilon,
            experiences: VecDeque::new(),
            max_experiences: 1000,
        }
    }

    pub fn get_q_value(&self, state: &str, action: &str) -> f64 {
        self.q_table
            .get(&(state.to_string(), action.to_string()))
            .copied()
            .unwrap_or(0.0)
    }

    pub fn set_q_value(&mut self, state: &str, action: &str, value: f64) {
        self.q_table.insert((state.to_string(), action.to_string()), value);
    }

    pub fn update_q_value(
        &mut self,
        state: &str,
        action: &str,
        reward: f64,
        next_state: &str,
        next_actions: &[&str],
    ) {
        let current_q = self.get_q_value(state, action);
        let max_next_q = next_actions
            .iter()
            .map(|a| self.get_q_value(next_state, a))
            .fold(f64::NEG_INFINITY, f64::max);

        let new_q = current_q
            + self.learning_rate * (reward + self.discount_factor * max_next_q - current_q);

        self.set_q_value(state, action, new_q);

        // Mise à jour des compteurs de visite
        *self.state_visits.entry(state.to_string()).or_insert(0) += 1;
        *self
            .action_visits
            .entry((state.to_string(), action.to_string()))
            .or_insert(0) += 1;
    }

    pub fn select_action(&self, state: &str, available_actions: &[&str]) -> String {
        if available_actions.is_empty() {
            return "default".to_string();
        }

        // Epsilon-greedy strategy
        if rand::random::<f64>() < self.epsilon {
            // Exploration : action aléatoire
            let idx = (rand::random::<f64>() * available_actions.len() as f64) as usize
                % available_actions.len();
            available_actions[idx].to_string()
        } else {
            // Exploitation : meilleure action connue
            available_actions
                .iter()
                .max_by(|a, b| {
                    self.get_q_value(state, **a)
                        .partial_cmp(&self.get_q_value(state, **b))
                        .unwrap_or(std::cmp::Ordering::Equal)
                })
                .map(|a| a.to_string())
                .unwrap_or_else(|| available_actions[0].to_string())
        }
    }

    pub fn record_experience(
        &mut self,
        state: String,
        action: String,
        reward: f64,
        next_state: String,
    ) {
        let experience = Experience {
            state,
            action,
            reward,
            next_state,
            timestamp: std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap_or_default()
                .as_secs(),
        };

        self.experiences.push_back(experience);
        if self.experiences.len() > self.max_experiences {
            self.experiences.pop_front();
        }
    }

    pub fn replay(&mut self, batch_size: usize) {
        let max_samples = std::cmp::min(batch_size, self.experiences.len());

        for _ in 0..max_samples {
            if let Some(exp) = self.experiences.pop_front() {
                let next_state_clone = exp.next_state.clone();
                let state_clone = exp.state.clone();
                let action_clone = exp.action.clone();
                let reward_val = exp.reward;
                
                let available_actions: Vec<String> = self.get_all_actions_for_state(&next_state_clone).iter().map(|s| s.to_string()).collect();
                self.update_q_value(&state_clone, &action_clone, reward_val, &next_state_clone, &available_actions.iter().map(|s| s.as_str()).collect::<Vec<_>>());
            }
        }
    }

    pub fn get_all_actions_for_state(&self, _state: &str) -> Vec<&str> {
        // Retourner les actions disponibles pour cet état
        // En pratique, cela dépend du problème spécifique
        vec!["action_1", "action_2", "action_3"]
    }

    pub fn get_best_policy(&self, state: &str) -> Option<(String, f64)> {
        self.q_table
            .iter()
            .filter(|((s, _), _)| s == state)
            .max_by(|a, b| a.1.partial_cmp(b.1).unwrap_or(std::cmp::Ordering::Equal))
            .map(|((_, action), q)| (action.clone(), *q))
    }

    pub fn get_learning_statistics(&self) -> HashMap<String, f64> {
        let mut stats = HashMap::new();

        let total_states = self.state_visits.len() as f64;
        let total_q_entries = self.q_table.len() as f64;
        let avg_q = self.q_table.values().sum::<f64>() / total_q_entries.max(1.0);

        stats.insert("total_states_visited".to_string(), total_states);
        stats.insert("total_state_action_pairs".to_string(), total_q_entries);
        stats.insert("average_q_value".to_string(), avg_q);
        stats.insert(
            "experience_buffer_size".to_string(),
            self.experiences.len() as f64,
        );

        stats
    }
}

impl Default for QLearningAgent {
    fn default() -> Self {
        Self::new(0.1, 0.99, 0.1)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_agent_creation() {
        let agent = QLearningAgent::new(0.1, 0.99, 0.1);
        assert_eq!(agent.learning_rate, 0.1);
        assert_eq!(agent.discount_factor, 0.99);
    }

    #[test]
    fn test_q_value_storage() {
        let mut agent = QLearningAgent::new(0.1, 0.99, 0.1);
        agent.set_q_value("s1", "a1", 5.0);
        assert_eq!(agent.get_q_value("s1", "a1"), 5.0);
    }

    #[test]
    fn test_action_selection() {
        let mut agent = QLearningAgent::new(0.1, 0.99, 0.0);
        agent.set_q_value("s1", "a1", 1.0);
        agent.set_q_value("s1", "a2", 5.0);

        let action = agent.select_action("s1", &["a1", "a2"]);
        assert_eq!(action, "a2");
    }

    #[test]
    fn test_experience_recording() {
        let mut agent = QLearningAgent::new(0.1, 0.99, 0.1);
        agent.record_experience("s1".to_string(), "a1".to_string(), 1.0, "s2".to_string());

        assert_eq!(agent.experiences.len(), 1);
    }

    #[test]
    fn test_learning_statistics() {
        let mut agent = QLearningAgent::new(0.1, 0.99, 0.1);
        agent.set_q_value("s1", "a1", 5.0);
        agent.set_q_value("s1", "a2", 3.0);

        let stats = agent.get_learning_statistics();
        assert!(stats.get("total_state_action_pairs").unwrap() > &0.0);
    }
}
