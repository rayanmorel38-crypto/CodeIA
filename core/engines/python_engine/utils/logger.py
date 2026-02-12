"""
Logger
------
Module simple de gestion des logs pour le python_engine offline.
"""

from typing import Optional

def run_task(task: dict = None, message: Optional[str] = None, level: str = 'INFO', *args, **kwargs) -> str:
    """
    Fonction standard pour executor.
    Génère un log formaté et le retourne.
    
    :param task: dict, informations sur la tâche (optionnel)
    :param message: str, message à logger
    :param level: str, niveau du log ('INFO', 'WARN', 'ERROR')
    :param args, kwargs: paramètres optionnels
    :return: str, log formaté
    """
    task_name = task.get('name') if task else "GLOBAL"
    log_message = f"[{level}] [{task_name}] {message}"
    return log_message

