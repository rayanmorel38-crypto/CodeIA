import unittest
from utils import logger, config_loader, timer
import json
import os

class TestUtils(unittest.TestCase):

    def test_logger_format(self):
        task = {'name': 'task1'}
        message = 'Test log message'
        log_str = logger.run_task(task=task, message=message, level='INFO')
        self.assertIn('[INFO]', log_str)
        self.assertIn('[task1]', log_str)
        self.assertIn(message, log_str)

    def test_config_loader(self):
        config_data = {'param1': 10, 'param2': 'value'}
        filepath = 'test_config.json'
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(config_data, f)
        loaded = config_loader.run_task(filepath=filepath)
        self.assertEqual(loaded, config_data)
        os.remove(filepath)

    def test_timer_context_manager(self):
        import time
        with timer.Timer() as t:
            time.sleep(0.1)
        self.assertTrue(t.elapsed >= 0.1)

    def test_timer_run_task(self):
        import time
        def dummy():
            time.sleep(0.05)
        elapsed = timer.run_task(func=dummy)
        self.assertTrue(elapsed >= 0.05)

if __name__ == '__main__':
    unittest.main()

