"""
Anomaly Detection
-----------------
Module pour détecter les anomalies dans les datasets.
"""

from typing import List, Any, Dict


def run_task(task: dict, data=None, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Détecte les anomalies dans les données.
    
    :param task: dict, informations sur la tâche
    :param data: list, dataset à analyser
    :return: dict, contient 'anomalies' et 'indices'
    """
    if data is None:
        data = task.get('data', [])
    
    method = task.get('method', 'zscore')
    threshold = task.get('threshold', 3.0)
    
    if method == 'zscore':
        if len(data) < 2:
            return {'anomalies': [], 'indices': []}
        
        mean = sum(data) / len(data)
        variance = sum((x - mean) ** 2 for x in data) / len(data)
        stddev = variance ** 0.5
        
        if stddev == 0:
            return {'anomalies': [], 'indices': []}
        
        anomalies = []
        indices = []
        for i, x in enumerate(data):
            z_score = abs((x - mean) / stddev)
            if z_score > threshold:
                anomalies.append(x)
                indices.append(i)
        
        return {'anomalies': anomalies, 'indices': indices}
    
    elif method == 'iqr':
        sorted_data = sorted(data)
        n = len(sorted_data)
        q1 = sorted_data[n//4]
        q3 = sorted_data[3*n//4]
        iqr = q3 - q1
        lower = q1 - 1.5 * iqr
        upper = q3 + 1.5 * iqr
        
        anomalies = []
        indices = []
        for i, x in enumerate(data):
            if x < lower or x > upper:
                anomalies.append(x)
                indices.append(i)
        
        return {'anomalies': anomalies, 'indices': indices}
    
    return {'anomalies': [], 'indices': []}

