"""
Statistics
----------
Module pour calculer les statistiques des datasets.
"""

from typing import List, Any, Optional, Dict


def run_task(task: dict, data=None, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Calcule les statistiques des données.
    
    :param task: dict, informations sur la tâche
    :param data: list, dataset pour lequel calculer les statistiques
    :return: dict, dictionnaire des statistiques
    """
    if data is None:
        data = task.get('data', [])
    
    if not data:
        return {}
    
    mean = sum(data) / len(data)
    variance = sum((x - mean) ** 2 for x in data) / len(data)
    stddev = variance ** 0.5
    sorted_data = sorted(data)
    n = len(sorted_data)
    median = sorted_data[n//2] if n % 2 else (sorted_data[n//2-1] + sorted_data[n//2]) / 2
    
    return {
        'mean': mean,
        'median': median,
        'stddev': stddev,
        'variance': variance,
        'min': min(data),
        'max': max(data),
        'count': len(data),
        'sum': sum(data)
    }

