import unittest
import json
import threading
import time
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import urllib.request
from server import HTTPServer, Handler, engine_pool


class TestServerAPI(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.server = HTTPServer(('127.0.0.1', 3004), Handler)
        cls.server_thread = threading.Thread(target=cls.server.serve_forever)
        cls.server_thread.daemon = True
        cls.server_thread.start()
        time.sleep(0.5)
    
    @classmethod
    def tearDownClass(cls):
        cls.server.shutdown()
    
    def test_health_endpoint(self):
        try:
            response = urllib.request.urlopen('http://127.0.0.1:3004/health')
            data = json.loads(response.read().decode())
            self.assertEqual(data['service'], 'python_engine')
            self.assertTrue(data['ready'])
        except Exception as e:
            self.fail(f"Health endpoint failed: {e}")
    
    def test_capabilities_endpoint(self):
        try:
            response = urllib.request.urlopen('http://127.0.0.1:3004/capabilities')
            data = json.loads(response.read().decode())
            self.assertEqual(data['language'], 'python')
            self.assertIn('pool_size', data)
        except Exception as e:
            self.fail(f"Capabilities endpoint failed: {e}")
    
    def test_stats_endpoint(self):
        try:
            response = urllib.request.urlopen('http://127.0.0.1:3004/stats')
            data = json.loads(response.read().decode())
            self.assertIn('total_tasks', data)
            self.assertIn('completed_tasks', data)
            self.assertIn('failed_tasks', data)
        except Exception as e:
            self.fail(f"Stats endpoint failed: {e}")
    
    def test_process_endpoint(self):
        try:
            task = {
                'action': 'run',
                'module': 'data_analysis',
                'function': 'cleaner',
                'data': [1, 2, None, 2]
            }
            
            data = json.dumps(task).encode('utf-8')
            req = urllib.request.Request(
                'http://127.0.0.1:3004/process',
                data=data,
                headers={'Content-Type': 'application/json'},
                method='POST'
            )
            
            response = urllib.request.urlopen(req)
            result = json.loads(response.read().decode())
            
            self.assertEqual(result['status'], 'accepted')
            self.assertIn('task_id', result)
            self.assertIn('status_url', result)
            
            task_id = result['task_id']
            self.assertTrue(task_id.startswith('task_'))
        except Exception as e:
            self.fail(f"Process endpoint failed: {e}")
    
    def test_status_endpoint(self):
        try:
            task = {
                'action': 'run',
                'module': 'data_analysis',
                'function': 'cleaner',
                'data': [1, 2, 3]
            }
            
            data = json.dumps(task).encode('utf-8')
            req = urllib.request.Request(
                'http://127.0.0.1:3004/process',
                data=data,
                headers={'Content-Type': 'application/json'},
                method='POST'
            )
            
            response = urllib.request.urlopen(req)
            result = json.loads(response.read().decode())
            task_id = result['task_id']
            
            time.sleep(0.5)
            
            status_response = urllib.request.urlopen(
                f'http://127.0.0.1:3004/status/{task_id}'
            )
            status_data = json.loads(status_response.read().decode())
            
            self.assertEqual(status_data['task_id'], task_id)
            self.assertIn('status', status_data)
            self.assertIn(status_data['status'], ['running', 'completed', 'failed'])
        except Exception as e:
            self.fail(f"Status endpoint failed: {e}")


if __name__ == '__main__':
    unittest.main()
