"""
Transformer
-----------
Module pour transformer et normaliser les datasets.
"""

import math
from typing import List, Any, Optional


def run_task(task: dict, data=None, **kwargs):
    """
    Fonction standard pour executor.
    Transforme les données selon le type spécifié.
    
    :param task: dict, informations sur la tâche
    :param data: list, dataset à transformer
    :return: list, dataset transformé
    """
    if data is None:
        data = task.get('data', [])
    
    transform_type = task.get('transform_type', 'normalize')
    
    if transform_type == 'normalize':
        if not data:
            return []
        min_val, max_val = min(data), max(data)
        return [(x - min_val) / (max_val - min_val) if max_val != min_val else 0 for x in data]
    
    elif transform_type == 'scale':
        if len(data) < 2:
            return data
        mean = sum(data) / len(data)
        variance = sum((x - mean) ** 2 for x in data) / len(data)
        stddev = variance ** 0.5
        return [(x - mean) / stddev if stddev != 0 else 0 for x in data]
    
    elif transform_type == 'log':
        return [math.log(max(x, 0.0001)) for x in data]
    
    elif transform_type == 'sqrt':
        return [x ** 0.5 if x >= 0 else 0 for x in data]
    
    elif transform_type == 'square':
        return [x ** 2 for x in data]
    
    elif transform_type == 'inverse':
        return [1/x if x != 0 else 0 for x in data]
    
    return data

