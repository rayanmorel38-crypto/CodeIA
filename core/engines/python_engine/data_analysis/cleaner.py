"""
Cleaner
-------
Module pour nettoyer et valider les datasets envoyés par l'orchestrator.
"""

from typing import List, Any

def run_task(task: dict, data: List[Any] = None, *args, **kwargs) -> List[Any]:
    """
    Fonction standard pour executor.
    Nettoie les données : supprime None et valeurs invalides.
    
    :param task: dict, informations sur la tâche
    :param data: list, dataset à nettoyer
    :return: list, dataset nettoyé
    """
    if data is None:
        data = task.get('data', [])
    cleaned = [d for d in data if d is not None]
    min_val = kwargs.get('min_value', None)
    max_val = kwargs.get('max_value', None)
    if min_val is not None:
        cleaned = [d for d in cleaned if d >= min_val]
    if max_val is not None:
        cleaned = [d for d in cleaned if d <= max_val]
    
    cleaned = list(dict.fromkeys(cleaned))
    
    cleaned = [d for d in cleaned if d != 0]
    
    cleaned = [d for d in cleaned if isinstance(d, (int, float))]
    
    return cleaned

