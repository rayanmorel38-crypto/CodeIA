"""
Tests for python_engine enhancements.
"""

import pytest
import time
from utils.task_runner import TaskRunner
from utils.enhanced_logger import logger
from utils.errors import EngineError, TaskError, TimeoutError

def test_task_runner_sync_success():
    """Test synchronous task execution."""
    runner = TaskRunner()
    
    def my_task(x: int) -> int:
        return x * 2
    
    result = runner.run_sync("test_sync", my_task, timeout_seconds=None, x=5)
    
    assert result.success
    assert result.output == 10
    assert result.name == "test_sync"
    assert result.duration_ms >= 0

def test_task_runner_sync_error():
    """Test error handling in synchronous tasks."""
    runner = TaskRunner()
    
    def failing_task():
        raise ValueError("Test error")
    
    result = runner.run_sync("test_error", failing_task)
    
    assert not result.success
    assert "Test error" in result.message

def test_task_runner_sync_timeout():
    """Test timeout handling."""
    runner = TaskRunner()
    
    def slow_task():
        time.sleep(2)
        return "done"
    
    result = runner.run_sync("test_timeout", slow_task, timeout_seconds=0.5)
    
    assert not result.success or result.duration_ms >= 500

def test_task_runner_multiple_tasks():
    """Test executing multiple tasks."""
    runner = TaskRunner()
    
    def task1():
        return "result1"
    
    def task2():
        return "result2"
    
    def task3():
        raise ValueError("task3 failed")
    
    runner.run_sync("task1", task1)
    runner.run_sync("task2", task2)
    runner.run_sync("task3", task3)
    
    results = runner.get_results()
    
    assert len(results) == 3
    assert results[0].success
    assert results[1].success
    assert not results[2].success
    
    summary = runner.get_summary()
    assert "3 total" in summary
    assert "2 succeeded" in summary
    assert "1 failed" in summary

def test_logger_creation():
    """Test logger singleton creation."""
    assert logger is not None
    logger.info("Test message")

def test_logger_levels():
    """Test different log levels."""
    logger.trace("Trace message")
    logger.debug("Debug message")
    logger.info("Info message")
    logger.warn("Warning message")
    logger.error("Error message")
    logger.critical("Critical message")

def test_errors():
    """Test custom error types."""
    try:
        raise TaskError("Task failed")
    except EngineError as e:
        assert "Task failed" in str(e)
    
    try:
        raise TimeoutError("Task timed out")
    except EngineError as e:
        assert "timed out" in str(e)

if __name__ == "__main__":
    pytest.main([__file__, "-v"])
