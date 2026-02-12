"""
Task runner with timeout support and result collection.
"""

import asyncio
from typing import Any, Callable, List, Optional
from dataclasses import dataclass
import time
from .errors import TimeoutError as EngineTimeoutError

@dataclass
class TaskResult:
    """Result of a task execution."""
    name: str
    success: bool
    duration_ms: float
    message: str
    output: Any = None

class TaskRunner:
    """Executes tasks with timeout support and collects results."""
    
    def __init__(self, max_concurrent: int = 4):
        self.max_concurrent = max_concurrent
        self.results: List[TaskResult] = []
    
    def run_sync(self, name: str, func: Callable, timeout_seconds: Optional[float] = None, *args: Any, **kwargs: Any) -> TaskResult:
        """
        Run a synchronous function with optional timeout.
        
        :param name: Task name
        :param func: Callable to execute
        :param timeout_seconds: Optional timeout in seconds
        :param args: Positional arguments for func
        :param kwargs: Keyword arguments for func
        :return: TaskResult
        """
        start = time.time()
        try:
            if timeout_seconds:
                result_container = [None]
                exception_container = [None]
                
                def wrapper():
                    try:
                        result_container[0] = func(*args, **kwargs)
                    except Exception as e:
                        exception_container[0] = e
                
                import threading
                thread = threading.Thread(target=wrapper)
                thread.daemon = True
                thread.start()
                thread.join(timeout=timeout_seconds)
                
                if thread.is_alive():
                    raise EngineTimeoutError(f"Task '{name}' exceeded timeout of {timeout_seconds}s")
                
                if exception_container[0]:
                    raise exception_container[0]
                
                output = result_container[0]
            else:
                output = func(*args, **kwargs)
            
            duration_ms = (time.time() - start) * 1000
            result = TaskResult(
                name=name,
                success=True,
                duration_ms=duration_ms,
                message="Task completed successfully",
                output=output
            )
        except Exception as e:
            duration_ms = (time.time() - start) * 1000
            result = TaskResult(
                name=name,
                success=False,
                duration_ms=duration_ms,
                message=str(e),
                output=None
            )
        
        self.results.append(result)
        return result
    
    async def run_async(self, name: str, coro: Callable, timeout_seconds: Optional[float] = None, *args: Any, **kwargs: Any) -> TaskResult:
        """
        Run an async coroutine with optional timeout.
        
        :param name: Task name
        :param coro: Async callable
        :param timeout_seconds: Optional timeout in seconds
        :param args: Positional arguments
        :param kwargs: Keyword arguments
        :return: TaskResult
        """
        start = time.time()
        try:
            if timeout_seconds:
                output = await asyncio.wait_for(
                    coro(*args, **kwargs),
                    timeout=timeout_seconds
                )
            else:
                output = await coro(*args, **kwargs)
            
            duration_ms = (time.time() - start) * 1000
            result = TaskResult(
                name=name,
                success=True,
                duration_ms=duration_ms,
                message="Task completed successfully",
                output=output
            )
        except asyncio.TimeoutError:
            duration_ms = (time.time() - start) * 1000
            result = TaskResult(
                name=name,
                success=False,
                duration_ms=duration_ms,
                message=f"Task exceeded timeout of {timeout_seconds}s",
                output=None
            )
        except Exception as e:
            duration_ms = (time.time() - start) * 1000
            result = TaskResult(
                name=name,
                success=False,
                duration_ms=duration_ms,
                message=str(e),
                output=None
            )
        
        self.results.append(result)
        return result
    
    def get_results(self) -> List[TaskResult]:
        """Get all collected results."""
        return self.results.copy()
    
    def clear_results(self) -> None:
        """Clear all results."""
        self.results.clear()
    
    def get_summary(self) -> str:
        """Get summary of execution."""
        total = len(self.results)
        succeeded = sum(1 for r in self.results if r.success)
        failed = total - succeeded
        total_time = sum(r.duration_ms for r in self.results)
        
        return (
            f"TaskRunner Summary: {total} total, "
            f"{succeeded} succeeded, {failed} failed, "
            f"{total_time:.2f}ms total"
        )
