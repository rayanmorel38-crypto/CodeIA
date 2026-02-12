"""
Dispatcher
----------
Décide quel module utiliser pour exécuter une tâche en fonction de son type et de ses critères.
"""

from typing import Dict, Any

class Dispatcher:
    def __init__(self):
        self.module_map = {
            'data': 'data_analysis',
            'simulation': 'simulations',
            'automation': 'automation',
            'ml': 'ml'
        }

    def register_module(self, task_type: str, module_name: str):
        """
        Permet de définir ou modifier le module responsable d'un type de tâche.
        :param task_type: str, type de tâche
        :param module_name: str, nom du module Python
        """
        self.module_map[task_type] = module_name

    def dispatch(self, task: Dict[str, Any]) -> str:
        """
        Retourne le module à utiliser pour exécuter une tâche.
        Si le type n'est pas reconnu, retourne 'utils'.
        :param task: dict contenant au moins 'name' et 'type'
        :return: str
        """
        task_type = task.get('type')
        if not task_type:
            raise ValueError("La tâche doit contenir une clé 'type'.")
        return self.module_map.get(task_type, 'utils')

    def dispatch_multiple(self, tasks: list) -> dict:
        """
        Retourne un mapping tâche -> module pour une liste de tâches.
        :param tasks: list de dict
        :return: dict
        """
        result = {}
        for task in tasks:
            module = self.dispatch(task)
            result[task['name']] = module
        return result

