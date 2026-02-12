"""
File Manager
-----------
Module pour gérer les opérations sur les fichiers et répertoires.
"""

import os
import shutil
from typing import Dict, Any, List


def run_task(task: dict, **kwargs) -> Dict[str, Any]:
    """
    Fonction standard pour executor.
    Effectue des opérations sur les fichiers.
    
    :param task: dict, informations sur la tâche
    :return: dict, résultat de l'opération
    """
    action = task.get('action', 'list')
    path = task.get('path', '.')
    
    if action == 'list':
        return os.listdir(path) if os.path.exists(path) else []
    
    elif action == 'copy':
        src = task.get('src')
        dst = task.get('dst')
        if os.path.isdir(src):
            shutil.copytree(src, dst)
        else:
            shutil.copy(src, dst)
        return {'status': 'copied'}
    
    elif action == 'delete':
        if os.path.isdir(path):
            shutil.rmtree(path)
        else:
            os.remove(path)
        return {'status': 'deleted'}
    
    elif action == 'rename':
        new_name = task.get('new_name')
        os.rename(path, new_name)
        return {'status': 'renamed'}
    
    elif action == 'exists':
        return {'exists': os.path.exists(path)}
    
    return {'status': 'unknown action'}

