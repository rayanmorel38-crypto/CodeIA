"""
Self Learning
-------------
Module pour Q-learning avec Q-table complète et epsilon-greedy.
"""

import random
from typing import Dict, Any, List, Tuple


class QLearningAgent:
    """Agent avec Q-learning complet."""
    
    def __init__(self, states: int = 100, actions: int = 4, learning_rate: float = 0.1, 
                 discount_factor: float = 0.95, epsilon: float = 0.1):
        """
        Initialise un agent Q-learning.
        
        :param states: nombre d'états
        :param actions: nombre d'actions
        :param learning_rate: taux d'apprentissage
        :param discount_factor: facteur d'actualisation
        :param epsilon: taux d'exploration (epsilon-greedy)
        """
        self.states = states
        self.actions = actions
        self.learning_rate = learning_rate
        self.discount_factor = discount_factor
        self.epsilon = epsilon
        
        self.q_table = [[0.0 for _ in range(actions)] for _ in range(states)]
    
    def select_action(self, state: int) -> int:
        """
        Sélectionne une action avec epsilon-greedy.
        
        :param state: état courant
        :return: action sélectionnée
        """
        if random.random() < self.epsilon:
            return random.randint(0, self.actions - 1)
        else:
            return self.q_table[state].index(max(self.q_table[state]))
    
    def update_q_value(self, state: int, action: int, reward: float, next_state: int) -> float:
        """
        Met à jour la Q-value avec l'équation de Q-learning.
        Q(s,a) = Q(s,a) + lr * (r + γ * max(Q(s',a')) - Q(s,a))
        
        :param state: état courant
        :param action: action prise
        :param reward: récompense reçue
        :param next_state: état suivant
        :return: nouvelle Q-value
        """
        max_next_q = max(self.q_table[next_state])
        old_q = self.q_table[state][action]
        
        new_q = old_q + self.learning_rate * (reward + self.discount_factor * max_next_q - old_q)
        self.q_table[state][action] = new_q
        
        return new_q
    
    def decay_epsilon(self, decay_rate: float = 0.995):
        """
        Diminue epsilon pour réduire l'exploration au fil du temps.
        
        :param decay_rate: taux de décroissance
        """
        self.epsilon *= decay_rate
        self.epsilon = max(self.epsilon, 0.01)


def run_task(task: dict, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Implémentation complète de Q-learning.
    
    :param task: dict, informations sur la tâche
    :return: dict, résultat de l'apprentissage
    """
    states = task.get('states', 100)
    actions = task.get('actions', 4)
    learning_rate = task.get('learning_rate', 0.1)
    discount_factor = task.get('discount_factor', 0.95)
    epsilon = task.get('epsilon', 0.1)
    
    agent = QLearningAgent(
        states=states,
        actions=actions,
        learning_rate=learning_rate,
        discount_factor=discount_factor,
        epsilon=epsilon
    )
    
    state = task.get('state', 0)
    action = task.get('action')
    reward = task.get('reward', 0)
    next_state = task.get('next_state', 0)
    
    if action is None:
        action = agent.select_action(state)
    
    new_q = agent.update_q_value(state, action, reward, next_state)
    
    if task.get('decay_epsilon', False):
        agent.decay_epsilon(task.get('decay_rate', 0.995))
    
    return {
        'state': state,
        'action': action,
        'reward': reward,
        'next_state': next_state,
        'new_q_value': new_q,
        'epsilon': agent.epsilon,
        'q_table_size': len(agent.q_table)
    }
