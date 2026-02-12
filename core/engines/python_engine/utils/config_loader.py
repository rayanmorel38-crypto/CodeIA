"""
Config Loader
-------------
Module pour charger des fichiers de configuration JSON pour le python_engine offline.
"""

import json
from typing import Optional, Any

def run_task(task: dict = None, filepath: Optional[str] = None, *args, **kwargs) -> Any:
    """
    Fonction standard pour executor.
    Charge et retourne le contenu d'un fichier JSON.
    
    :param task: dict, informations sur la tâche (optionnel)
    :param filepath: str, chemin du fichier JSON
    :param args, kwargs: paramètres optionnels
    :return: contenu du fichier JSON sous forme de dict
    """
    if filepath is None:
        if task is None or 'filepath' not in task:
            raise ValueError("Le chemin du fichier doit être fourni via 'filepath'.")
        filepath = task['filepath']

    with open(filepath, 'r', encoding='utf-8') as f:
        return json.load(f)

