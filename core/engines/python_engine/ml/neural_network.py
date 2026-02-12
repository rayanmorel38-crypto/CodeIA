"""
Neural Network
--------------
Module pour simulation simple de réseaux de neurones.
"""

import numpy as np
import json
import os
from typing import Dict, Any, List


def run_task(task: dict, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Simule un réseau de neurones simple.
    
    :param task: dict, informations sur la tâche
    :return: dict, contient 'output' et 'weights'
    """
    data = task.get('data', [])
    weights = task.get('weights', [1.0] * len(data) if data else [])
    
    output = sum(w * x for w, x in zip(weights, data))
    
    return {
        'output': output,
        'weights': weights
    }
