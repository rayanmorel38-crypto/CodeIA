"""
HTTP client for orchestrator communication.
"""

import requests
import json
from typing import Optional, Dict, Any
from .errors import IntegrationError
from .enhanced_logger import logger

class OrchestratorClient:
    """Client for communicating with orchestrator service."""
    
    def __init__(self, host: str = "127.0.0.1", port: int = 8000):
        self.base_url = f"http://{host}:{port}"
        self.timeout = 30
    
    def health_check(self) -> bool:
        """Check if orchestrator is healthy."""
        try:
            response = requests.get(
                f"{self.base_url}/health",
                timeout=5
            )
            return response.status_code == 200
        except requests.RequestException as e:
            logger.error(f"Health check failed: {e}")
            return False
    
    def analyze(self, code: str, language: str = "python") -> Dict[str, Any]:
        """Send code for analysis."""
        try:
            response = requests.post(
                f"{self.base_url}/analyze",
                json={
                    "code": code,
                    "language": language,
                    "timeout_seconds": 30
                },
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            logger.error(f"Analysis request failed: {e}")
            raise IntegrationError(f"Failed to analyze code: {e}")
    
    def optimize(self, code: str) -> Dict[str, Any]:
        """Request code optimization."""
        try:
            response = requests.post(
                f"{self.base_url}/optimize",
                json={"code": code},
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            logger.error(f"Optimization request failed: {e}")
            raise IntegrationError(f"Failed to optimize code: {e}")
    
    def execute(self, task_type: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """Execute a generic task on orchestrator."""
        try:
            response = requests.post(
                f"{self.base_url}/{task_type}",
                json=data,
                timeout=self.timeout
            )
            response.raise_for_status()
            return response.json()
        except requests.RequestException as e:
            logger.error(f"Task execution failed: {e}")
            raise IntegrationError(f"Task execution failed: {e}")
