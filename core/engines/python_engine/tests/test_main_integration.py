import unittest
from core import task_manager, dispatcher, executor
from data_analysis import cleaner, transformer

class TestIntegration(unittest.TestCase):

    def setUp(self):
        self.tm = task_manager.TaskManager()
        self.dispatcher = dispatcher.Dispatcher()
        self.executor = executor.Executor()

        self.dispatcher.register_module('clean', 'data_analysis.cleaner')
        self.dispatcher.register_module('transform', 'data_analysis.transformer')

        self.tm.add_task({'name': 'clean_task', 'type': 'clean', 'priority': 1, 'data': [1, None, 2, 3]})
        self.tm.add_task({'name': 'transform_task', 'type': 'transform', 'priority': 2, 'data': [1, 2, 3]})

    def test_workflow_execution(self):
        tasks = self.tm.get_tasks()
        results = {}
        for t in tasks:
            module_name = self.dispatcher.dispatch(t)
            result = self.executor.execute(module_name, t)
            results[t['name']] = result

        self.assertEqual(results['clean_task'], [1, 2, 3])
        self.assertEqual(results['transform_task'], [0.0, 0.5, 1.0])

if __name__ == '__main__':
    unittest.main()

