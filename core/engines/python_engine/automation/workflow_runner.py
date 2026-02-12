"""
Workflow Runner
---------------
Module pour exécuter des séquences de tâches (workflows).
"""

from typing import Dict, Any, Optional


def run_task(task: dict, executor=None, dispatcher=None, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Exécute une séquence de tâches.
    
    :param task: dict, informations sur la tâche
    :param executor: objet executor pour exécuter les étapes
    :param dispatcher: objet dispatcher pour router les étapes
    :return: dict, résultat du workflow
    """
    workflow_name = task.get('name', 'workflow')
    steps = task.get('steps', [])
    
    results = {}
    for step in steps:
        step_name = step.get('name', 'unknown')
        step_type = step.get('type')
        
        module_name = dispatcher.dispatch(step) if dispatcher else None
        if module_name and executor:
            result = executor.execute(module_name, step)
            results[step_name] = result
        else:
            results[step_name] = None
    
    return {
        'workflow': workflow_name,
        'results': results,
        'total_steps': len(steps),
        'completed': len([r for r in results.values() if r is not None])
    }

