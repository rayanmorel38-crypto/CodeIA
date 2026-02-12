"""
TaskManager
------------
Gère les tâches envoyées par l'orchestrator.
Stocke, ajoute et récupère les tâches avec priorités et statut.
"""

from enum import Enum

class TaskStatus(Enum):
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"

class TaskManager:
    def __init__(self):
        self.tasks = []

    def add_task(self, task):
        """
        Ajoute une tâche à la liste.
        :param task: dict contenant au moins 'name', 'type' et optionnel 'priority' (int)
        """
        if not isinstance(task, dict) or 'name' not in task or 'type' not in task:
            raise ValueError("Une tâche doit être un dict avec 'name' et 'type'.")
        
        task_entry = dict(task)
        task_entry['priority'] = task.get('priority', 0)
        task_entry['status'] = TaskStatus.PENDING
        
        self.tasks.append(task_entry)
        self.tasks.sort(key=lambda t: t['priority'], reverse=True)

    def get_tasks(self, status=None, task_type=None):
        """
        Retourne toutes les tâches filtrées par statut et/ou type.
        :param status: TaskStatus ou liste de TaskStatus
        :param task_type: str ou liste de str
        :return: list
        """
        filtered = self.tasks
        if status:
            if not isinstance(status, list):
                status = [status]
            filtered = [t for t in filtered if t['status'] in status]
        if task_type:
            if not isinstance(task_type, list):
                task_type = [task_type]
            filtered = [t for t in filtered if t['type'] in task_type]
        return filtered

    def update_status(self, task_name, status):
        """
        Met à jour le statut d'une tâche.
        :param task_name: nom de la tâche
        :param status: TaskStatus
        """
        for t in self.tasks:
            if t['name'] == task_name:
                t['status'] = status
                return
        raise ValueError(f"Tâche '{task_name}' non trouvée.")

    def remove_task(self, task_name):
        """
        Supprime une tâche de la liste par nom.
        :param task_name: str
        """
        self.tasks = [t for t in self.tasks if t['name'] != task_name]

    def clear_tasks(self):
        """Vide toutes les tâches."""
        self.tasks = []

