"""
Server léger pour python_engine en mode orchestrateur.

Endpoints:
 - GET /health
 - POST /process
 - GET /status/{task_id}
"""

import argparse
import json
import os
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse

ROOT = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(ROOT, '..', '..', '..'))
if PROJECT_ROOT not in sys.path:
    sys.path.insert(0, PROJECT_ROOT)

from core.engines.python_engine.main import PythonEngine


class EngineRuntime:
    def __init__(self):
        self.engine = PythonEngine()
        self._executor = ThreadPoolExecutor(max_workers=1)
        self._futures = {}
        self._tasks = {}
        self._lock = threading.Lock()
        self._engine_lock = threading.Lock()
        self._task_counter = 0

    def _next_task_id(self) -> str:
        with self._lock:
            self._task_counter += 1
            return f"task_{self._task_counter}_{int(time.time() * 1000)}"

    def _run_task(self, task_id: str, task: dict):
        started_at = time.time()
        with self._lock:
            self._tasks[task_id]['started_at'] = started_at

        with self._engine_lock:
            self.engine.add_task(task)
            result = self.engine.run_all()

        completed_at = time.time()
        with self._lock:
            self._tasks[task_id]['completed_at'] = completed_at
        return result

    def submit_task_async(self, task: dict) -> str:
        task_id = self._next_task_id()
        created_at = time.time()

        with self._lock:
            self._tasks[task_id] = {
                'created_at': created_at,
                'started_at': None,
                'completed_at': None
            }
            self._futures[task_id] = self._executor.submit(self._run_task, task_id, task)

        return task_id

    def execute_sync(self, task: dict):
        task_id = self._next_task_id()
        created_at = time.time()
        started_at = time.time()

        with self._engine_lock:
            self.engine.add_task(task)
            result = self.engine.run_all()

        completed_at = time.time()
        metrics = {
            'task_id': task_id,
            'created_at': created_at,
            'started_at': started_at,
            'completed_at': completed_at,
            'duration_ms': int((completed_at - started_at) * 1000)
        }
        return result, metrics

    def get_task_status(self, task_id: str):
        with self._lock:
            future = self._futures.get(task_id)
            meta = self._tasks.get(task_id)

        if future is None or meta is None:
            return None

        created_at = meta.get('created_at')
        started_at = meta.get('started_at')
        completed_at = meta.get('completed_at')
        metrics = {
            'task_id': task_id,
            'created_at': created_at,
            'started_at': started_at,
            'completed_at': completed_at,
            'duration_ms': int((completed_at - started_at) * 1000) if started_at and completed_at else None
        }

        if future.cancelled():
            return {
                'status': 'cancelled',
                'result': None,
                'error': 'Task was cancelled',
                'metrics': metrics
            }

        if not future.done():
            status = 'queued' if started_at is None else 'running'
            return {
                'status': status,
                'result': None,
                'error': None,
                'metrics': metrics
            }

        exc = future.exception()
        if exc is not None:
            return {
                'status': 'failed',
                'result': None,
                'error': str(exc),
                'metrics': metrics
            }

        return {
            'status': 'completed',
            'result': future.result(),
            'error': None,
            'metrics': metrics
        }

    def get_metrics_snapshot(self):
        with self._lock:
            total = len(self._tasks)
            queued = 0
            running = 0
            completed = 0
            failed = 0

            for task_id, future in self._futures.items():
                meta = self._tasks.get(task_id, {})
                if future.cancelled():
                    failed += 1
                elif not future.done():
                    if meta.get('started_at') is None:
                        queued += 1
                    else:
                        running += 1
                elif future.exception() is not None:
                    failed += 1
                else:
                    completed += 1

        return {
            'total_tasks': total,
            'queued_tasks': queued,
            'running_tasks': running,
            'completed_tasks': completed,
            'failed_tasks': failed
        }


engine_runtime = EngineRuntime()
ORCHESTRATOR_TOKEN = os.getenv('CODEIA_ORCHESTRATOR_TOKEN', '').strip()


class Handler(BaseHTTPRequestHandler):
    def _send_json(self, data, status=200):
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode('utf-8'))

    def _response(self, status: str, result=None, error=None, metrics=None):
        return {
            'status': status,
            'result': result,
            'error': error,
            'metrics': metrics or {}
        }

    def _require_orchestrator_token(self) -> bool:
        token = self.headers.get('X-Orchestrator-Token', '').strip()
        if not token or token != ORCHESTRATOR_TOKEN:
            self._send_json(self._response('unauthorized', None, 'Unauthorized', {}), status=401)
            return False
        return True

    def do_GET(self):
        if not self._require_orchestrator_token():
            return

        parsed = urlparse(self.path)
        if parsed.path == '/health':
            self._send_json(self._response(
                status='ok',
                result={'service': 'python_engine', 'ready': True},
                error=None,
                metrics=engine_runtime.get_metrics_snapshot()
            ))
        elif parsed.path.startswith('/status/'):
            task_id = parsed.path.split('/status/')[-1]
            response = engine_runtime.get_task_status(task_id)
            if response is None:
                self._send_json(self._response(
                    status='not_found',
                    result=None,
                    error=f'Task not found: {task_id}',
                    metrics={'task_id': task_id}
                ), status=404)
                return
            self._send_json(response)
        else:
            self.send_error(404, 'Not Found')

    def do_POST(self):
        if not self._require_orchestrator_token():
            return

        parsed = urlparse(self.path)
        if parsed.path == '/process':
            length = int(self.headers.get('Content-Length', 0))
            body = self.rfile.read(length)
            try:
                task = json.loads(body.decode('utf-8'))
            except Exception as exc:
                self._send_json(self._response(
                    status='invalid_request',
                    result=None,
                    error=f'Invalid JSON: {str(exc)}',
                    metrics={}
                ), status=400)
                return

            async_mode = bool(task.get('async', True))
            if async_mode:
                task_id = engine_runtime.submit_task_async(task)
                self._send_json(self._response(
                    status='accepted',
                    result={
                        'task_id': task_id,
                        'status_url': f'/status/{task_id}'
                    },
                    error=None,
                    metrics={'task_id': task_id}
                ), status=202)
            else:
                try:
                    result, metrics = engine_runtime.execute_sync(task)
                    self._send_json(self._response(
                        status='completed',
                        result=result,
                        error=None,
                        metrics=metrics
                    ))
                except Exception as exc:
                    self._send_json(self._response(
                        status='failed',
                        result=None,
                        error=str(exc),
                        metrics={}
                    ), status=500)
        else:
            self.send_error(404, 'Not Found')


def run(host='127.0.0.1', port=3003):
    launch_mode = os.getenv('CODEIA_LAUNCH_MODE', '').strip()
    if launch_mode != 'orchestrator':
        raise RuntimeError("Startup refused: CODEIA_LAUNCH_MODE must be 'orchestrator'.")
    if not ORCHESTRATOR_TOKEN:
        raise RuntimeError("Startup refused: CODEIA_ORCHESTRATOR_TOKEN is required.")

    server = HTTPServer((host, port), Handler)
    print(f"python_engine control server running on http://{host}:{port}")
    server.serve_forever()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Python Engine Control Server')
    parser.add_argument('--port', type=int, default=3003, help='Port to run the server on')
    parser.add_argument('--host', type=str, default='127.0.0.1', help='Host to bind the server to')
    args = parser.parse_args()
    run(host=args.host, port=args.port)
