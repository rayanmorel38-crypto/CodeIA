"""
LLM Agent
---------
Module pour simuler les interactions LLM simples (sans API réelle).
"""

from typing import Dict, Any


def run_task(task: dict, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Simule une interaction LLM.
    
    :param task: dict, informations sur la tâche
    :return: dict, contient 'response' et 'prompt'
    """
    prompt = task.get('prompt', '')
    task_type = task.get('task_type', 'chat')
    
    responses = {
        'summarize': f"Résumé de: {prompt[:50]}...",
        'translate': f"Traduction (mock): {prompt}",
        'generate': f"Généré basé sur: {prompt}",
        'chat': f"Réponse à: {prompt}"
    }
    
    return {
        'response': responses.get(task_type, "Unknown task"),
        'prompt': prompt
    }