import unittest
import json
import time
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from server import EnginePool


class TestTaskTracking(unittest.TestCase):
    def setUp(self):
        self.pool = EnginePool(pool_size=2)
    
    def test_task_id_generation(self):
        task_id_1 = self.pool.generate_task_id()
        task_id_2 = self.pool.generate_task_id()
        self.assertNotEqual(task_id_1, task_id_2)
        self.assertTrue(task_id_1.startswith('task_'))
        self.assertTrue(task_id_2.startswith('task_'))
    
    def test_task_submission_async(self):
        task = {'action': 'run', 'module': 'data_analysis', 'function': 'cleaner'}
        task_id, future = self.pool.submit_task_async(task)
        
        self.assertIsNotNone(task_id)
        self.assertTrue(task_id.startswith('task_'))
        self.assertIsNotNone(future)
    
    def test_task_status_tracking(self):
        task = {'action': 'run', 'module': 'data_analysis', 'function': 'cleaner', 'data': [1, 2, None, 2]}
        task_id, future = self.pool.submit_task_async(task)
        
        time.sleep(0.1)
        
        status = self.pool.get_task_status(task_id)
        self.assertIn('task_id', status)
        self.assertIn('status', status)
        self.assertIn('created_at', status)
        self.assertEqual(status['task_id'], task_id)
    
    def test_task_not_found(self):
        status = self.pool.get_task_status('task_nonexistent')
        self.assertIn('error', status)
        self.assertEqual(status['error'], 'Task not found')
    
    def test_multiple_concurrent_tasks(self):
        tasks = [
            {'action': 'run', 'module': 'data_analysis', 'function': 'cleaner', 'data': [1, 2, None, 2]} for _ in range(3)
        ]
        
        task_ids = []
        futures = []
        for task in tasks:
            task_id, future = self.pool.submit_task_async(task)
            task_ids.append(task_id)
            futures.append(future)
        
        self.assertEqual(len(set(task_ids)), 3)
        
        time.sleep(0.5)
        
        for task_id in task_ids:
            status = self.pool.get_task_status(task_id)
            self.assertIn('status', status)
            self.assertIn(status['status'], ['completed', 'failed', 'running'])
    
    def test_pool_stats_update(self):
        initial_stats = self.pool.get_stats()
        initial_total = initial_stats['total_tasks']
        
        task = {'action': 'run', 'module': 'data_analysis', 'function': 'cleaner', 'data': [1, 2, 3]}
        task_id, future = self.pool.submit_task_async(task)
        
        time.sleep(0.1)
        
        updated_stats = self.pool.get_stats()
        self.assertEqual(updated_stats['total_tasks'], initial_total + 1)


if __name__ == '__main__':
    unittest.main()
