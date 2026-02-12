"""
Timer
-----
Module pour mesurer le temps d'exécution des tâches dans le python_engine offline.
"""

import time
from typing import Optional

class Timer:
    """
    Context manager pour mesurer la durée d'exécution.
    Utilisable avec 'with Timer() as t: ...'
    """

    def __init__(self):
        self.start: Optional[float] = None
        self.end: Optional[float] = None
        self.elapsed: Optional[float] = None

    def __enter__(self):
        self.start = time.time()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.end = time.time()
        self.elapsed = self.end - self.start

def run_task(task: dict = None, func: Optional[callable] = None, *args, **kwargs) -> float:
    """
    Fonction standard pour executor.
    Mesure le temps d'exécution d'une fonction fournie.
    
    :param task: dict, informations sur la tâche (optionnel)
    :param func: callable, fonction à mesurer
    :param args, kwargs: paramètres à passer à la fonction
    :return: float, temps écoulé en secondes
    """
    if func is None:
        raise ValueError("Une fonction 'func' doit être fournie pour le timing.")

    start_time = time.time()
    func(*args, **kwargs)
    end_time = time.time()
    return end_time - start_time

