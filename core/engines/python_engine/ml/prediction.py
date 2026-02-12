"""
Prediction
----------
Module pour effectuer des prédictions simples sur des datasets locaux offline.
"""

from typing import List, Any, Callable, Optional

def run_task(task: dict, model: Optional[Callable[[Any], Any]] = None, data: Optional[List[Any]] = None, *args, **kwargs) -> List[Any]:
    """
    Fonction standard pour executor.
    Applique le modèle fourni sur le dataset.
    
    :param task: dict, informations sur la tâche
    :param model: fonction callable qui prend une donnée et retourne la prédiction
    :param data: liste de données à prédire
    :param args, kwargs: paramètres optionnels
    :return: liste des prédictions
    """
    if data is None:
        data = task.get('data', [])

    if model is None:
        a = task.get('slope', 1.0)
        b = task.get('intercept', 0.0)
        return [a * x + b for x in data]

    return [model(d) for d in data]

