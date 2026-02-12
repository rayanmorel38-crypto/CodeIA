"""
Report Generator
----------------
Module pour générer des rapports à partir des données.
"""

from typing import Dict, Any, List
import json


def run_task(task: dict, data=None, **kwargs) -> str:
    """
    Fonction standard pour executor.
    Génère un rapport à partir des données.
    
    :param task: dict, informations sur la tâche
    :param data: dict ou list, données pour le rapport
    :return: str, rapport formaté
    """
    if data is None:
        data = task.get('data', {})
    
    title = task.get('title', 'Report')
    format_type = task.get('format', 'text')
    
    if format_type == 'text':
        report = f"=== {title} ===\n\n"
        for key, value in data.items():
            report += f"{key}: {value}\n"
        return report
    
    elif format_type == 'json':
        return json.dumps({'title': title, 'data': data}, indent=2)
    
    elif format_type == 'csv':
        rows = task.get('rows', [])
        report = ",".join(str(k) for k in rows[0].keys()) + "\n" if rows else ""
        for row in rows:
            report += ",".join(str(v) for v in row.values()) + "\n"
        return report
    
    return f"Report: {title}\n{data}"

