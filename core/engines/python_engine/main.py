"""
Main
----
Point d'entrée du python_engine offline.
Orchestre l'exécution des tâches via TaskManager, Dispatcher et Executor.
"""

from core.engines.python_engine.core import task_manager, dispatcher, executor
from core.engines.python_engine.data_analysis import cleaner, transformer, aggregator, statistics
from core.engines.python_engine.ml import prediction, anomaly_detection, llm_agent
from core.engines.python_engine.automation import report_generator, workflow_runner, file_manager

class PythonEngine:
    def __init__(self):
        self.tm = task_manager.TaskManager()
        self.dispatcher = dispatcher.Dispatcher()
        self.executor = executor.Executor()

        self.dispatcher.register_module('clean', 'core.engines.python_engine.data_analysis.cleaner')
        self.dispatcher.register_module('transform', 'core.engines.python_engine.data_analysis.transformer')
        self.dispatcher.register_module('aggregate', 'core.engines.python_engine.data_analysis.aggregator')
        self.dispatcher.register_module('statistics', 'core.engines.python_engine.data_analysis.statistics')
        self.dispatcher.register_module('predict', 'core.engines.python_engine.ml.prediction')
        self.dispatcher.register_module('anomaly', 'core.engines.python_engine.ml.anomaly_detection')
        self.dispatcher.register_module('llm_agent', 'core.engines.python_engine.ml.llm_agent')
        self.dispatcher.register_module('report', 'core.engines.python_engine.automation.report_generator')
        self.dispatcher.register_module('workflow', 'core.engines.python_engine.automation.workflow_runner')
        self.dispatcher.register_module('file', 'core.engines.python_engine.automation.file_manager')
        self.dispatcher.register_module('code_generator', 'core.engines.python_engine.automation.code_generator')

    def add_task(self, task: dict):
        """
        Ajoute une tâche à exécuter.
        :param task: dict avec 'name', 'type', et éventuellement 'data' ou 'priority'
        """
        self.tm.add_task(task)

    def run_all(self):
        """
        Exécute toutes les tâches présentes dans le TaskManager.
        :return: dict {nom_tâche: résultat}
        """
        results = {}
        tasks = self.tm.get_tasks()
        for t in tasks:
            module_name = self.dispatcher.dispatch(t)
            result = self.executor.execute(module_name, t)
            results[t['name']] = result
        return results

    def run_workflow(self, tasks_list):
        """
        Exécute un workflow complet via workflow_runner.
        :param tasks_list: liste de tâches
        :return: dict {nom_tâche: résultat}
        """
        return workflow_runner.run_task(
            task=None,
            executor=self.executor,
            dispatcher=self.dispatcher,
            tasks_list=tasks_list
        )

    def clear_tasks(self):
        """Vide toutes les tâches stockées."""
        self.tm.clear_tasks()


if __name__ == '__main__':
    engine = PythonEngine()

    engine.add_task({'name': 'clean_task', 'type': 'clean', 'data': [1, None, 2, 3], 'priority': 2})
    engine.add_task({'name': 'transform_task', 'type': 'transform', 'data': [1, 2, 3], 'priority': 1})
    engine.add_task({'name': 'aggregate_task', 'type': 'aggregate', 'data': [1, 2, 3], 'priority': 1})

    results = engine.run_all()

    workflow_results = engine.run_workflow(engine.tm.get_tasks())

    engine.clear_tasks()

