"""
Aggregator
----------
Module pour agréger les datasets (sum, mean, count, etc.).
"""

from typing import List, Any, Optional


def run_task(task: dict, data=None, **kwargs):
    """
    Fonction standard pour executor.
    Agrège les données selon le type spécifié.
    
    :param task: dict, informations sur la tâche
    :param data: list, dataset à agréger
    :return: valeur agrégée
    """
    if data is None:
        data = task.get('data', [])
    
    agg_type = task.get('aggregation', 'sum')
    
    if agg_type == 'sum':
        return sum(data)
    elif agg_type == 'count':
        return len(data)
    elif agg_type == 'avg' or agg_type == 'mean':
        return sum(data) / len(data) if data else 0
    elif agg_type == 'min':
        return min(data) if data else None
    elif agg_type == 'max':
        return max(data) if data else None
    elif agg_type == 'median':
        sorted_data = sorted(data)
        n = len(sorted_data)
        return sorted_data[n//2] if n % 2 else (sorted_data[n//2-1] + sorted_data[n//2]) / 2
    elif agg_type == 'stddev':
        mean = sum(data) / len(data) if data else 0
        variance = sum((x - mean) ** 2 for x in data) / len(data) if data else 0
        return variance ** 0.5
    
    return None

