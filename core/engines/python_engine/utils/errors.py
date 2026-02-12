"""
Custom exceptions for python_engine.
"""

class EngineError(Exception):
    """Base exception for engine errors."""
    def __init__(self, message, code=None, context=None):
        self.message = message
        self.code = code or 'ENGINE_ERROR'
        self.context = context or {}
        super().__init__(message)

class TaskError(EngineError):
    """Raised when task execution fails."""
    pass

class DispatchError(EngineError):
    """Raised when dispatcher cannot find module."""
    pass

class ExecutorError(EngineError):
    """Raised when executor fails to run task."""
    pass

class TimeoutError(EngineError):
    """Raised when task execution times out."""
    pass

class ConfigError(EngineError):
    """Raised when configuration is invalid."""
    pass

class IntegrationError(EngineError):
    """Raised when orchestrator integration fails."""
    pass

class ValidationError(EngineError):
    """Raised when task validation fails."""
    pass

class PoolExhaustedError(EngineError):
    """Raised when all engine instances are exhausted."""
    pass

class TaskNotFoundError(EngineError):
    """Raised when task_id does not exist."""
    pass

class InvalidTaskError(EngineError):
    """Raised when task has invalid format."""
    pass

class ModuleNotFoundError(EngineError):
    """Raised when required module cannot be loaded."""
    pass
