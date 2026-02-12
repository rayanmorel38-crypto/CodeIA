"""
Executor
--------
Exécute les tâches en utilisant le module Python approprié.
Ne gère pas la logique métier elle-même, se contente d'appeler les modules ciblés.
"""

import importlib
from typing import Dict, Any

class Executor:
    def __init__(self):
        self._modules_cache = {}

    def _load_module(self, module_name: str):
        """
        Charge dynamiquement un module Python à partir de son nom.
        :param module_name: str, nom du module à importer
        :return: module Python
        """
        if module_name in self._modules_cache:
            return self._modules_cache[module_name]
        try:
            module = importlib.import_module(module_name)
            self._modules_cache[module_name] = module
            return module
        except ModuleNotFoundError:
            raise ImportError(f"Le module '{module_name}' n'existe pas ou n'est pas installé.")

    def execute(self, module_name: str, task: Dict[str, Any], *args, **kwargs):
        """
        Exécute la tâche en appelant la fonction 'run_task' du module ciblé.
        :param module_name: str, nom du module à utiliser
        :param task: dict, tâche à exécuter
        :param args: arguments supplémentaires à passer à run_task
        :param kwargs: arguments supplémentaires à passer à run_task
        :return: résultat de la tâche
        """
        module = self._load_module(module_name)
        if not hasattr(module, 'run_task'):
            raise AttributeError(f"Le module '{module_name}' doit définir une fonction 'run_task(task, *args, **kwargs)'")
        result = module.run_task(task, *args, **kwargs)
        return result

    def execute_batch(self, tasks_mapping: Dict[str, str], *args, **kwargs) -> Dict[str, Any]:
        """
        Exécute plusieurs tâches en batch.
        :param tasks_mapping: dict {task_name: module_name}
        :return: dict {task_name: result}
        """
        results = {}
        for task_name, module_name in tasks_mapping.items():
            task = {'name': task_name, 'type': None}
            results[task_name] = self.execute(module_name, task, *args, **kwargs)
        return results

