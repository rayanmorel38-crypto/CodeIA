"""
Code Generator
--------------
Module pour générer du code dans différents langages de programmation.
"""

import re
from typing import Dict, Any, Optional

def run_task(task: Dict[str, Any], executor=None, dispatcher=None, tasks_list=None) -> Dict[str, Any]:
    """
    Génère du code basé sur la spécification donnée.
    
    :param task: dict avec 'language', 'type' (function/class/interface), 'name', 'description'
    :return: dict avec le code généré
    """
    language = task.get('language', 'python').lower()
    code_type = task.get('type', 'function')
    name = task.get('name', 'example')
    description = task.get('description', 'Generated code')
    
    if language == 'python':
        if code_type == 'function':
            code = generate_python_function(name, description)
        elif code_type == 'class':
            code = generate_python_class(name, description)
        else:
            code = f"Python code for {name}\n{description}\npass"
            
    elif language == 'javascript':
        if code_type == 'function':
            code = generate_javascript_function(name, description)
        elif code_type == 'class':
            code = generate_javascript_class(name, description)
        else:
            code = f"// JavaScript code for {name}\n// {description}\n// TODO: implement"
            
    elif language == 'java':
        if code_type == 'interface':
            code = generate_java_interface(name, description)
        elif code_type == 'class':
            code = generate_java_class(name, description)
        else:
            code = f"// Java code for {name}\n// {description}\npublic class {name} {{\n    // TODO: implement\n}}"
            
    else:
        code = f"// Generated code for {language}\n// {description}\n// Language not fully supported"
    
    return {
        'language': language,
        'type': code_type,
        'name': name,
        'code': code,
        'description': description
    }

def generate_python_function(name: str, description: str) -> str:
    """Génère une fonction Python simple."""
    return f'''"""
{description}
"""
def {name}():
    """
    {description}
    """
    pass

if __name__ == "__main__":
    result = {name}()
    print(f"Result: {{result}}")'''

def generate_python_class(name: str, description: str) -> str:
    """Génère une classe Python simple."""
    return f'''"""
{description}
"""
class {name}:
    """
    {description}
    """
    
    def __init__(self):
        """Initialize the {name} class."""
        self.data = []
    
    def add_item(self, item):
        """Add an item to the collection."""
        self.data.append(item)
    
    def get_items(self):
        """Get all items."""
        return self.data.copy()

if __name__ == "__main__":
    instance = {name}()
    instance.add_item("example")
    print(f"Items: {{instance.get_items()}}")'''

def generate_javascript_function(name: str, description: str) -> str:
    """Génère une fonction JavaScript simple."""
    return f'''/**
 * {description}
 */
function {name}() {{
    // {description}
    // TODO: Implement the function logic
    return null;
}}

// Example usage
if (typeof module !== 'undefined' && module.exports) {{
    module.exports = {{ {name} }};
}} else {{
    // Browser usage
    window.{name} = {name};
}}'''

def generate_javascript_class(name: str, description: str) -> str:
    """Génère une classe JavaScript simple."""
    return f'''/**
 * {description}
 */
class {name} {{
    /**
     * {description}
     */
    constructor() {{
        this.data = [];
    }}
    
    /**
     * Add an item to the collection
     * @param {{any}} item - The item to add
     */
    addItem(item) {{
        this.data.push(item);
    }}
    
    /**
     * Get all items
     * @returns {{Array}} All items
     */
    getItems() {{
        return [...this.data];
    }}
}}

// Example usage
const instance = new {name}();
instance.addItem("example");
console.log("Items:", instance.getItems());'''

def generate_java_interface(name: str, description: str) -> str:
    """Génère une interface Java simple."""
    return f'''/**
 * {description}
 */
public interface {name} {{
    
    /**
     * {description}
     */
    void execute();
    
    /**
     * Get the result of execution
     * @return the result
     */
    Object getResult();
}}'''

def generate_java_class(name: str, description: str) -> str:
    """Génère une classe Java simple."""
    return f'''/**
 * {description}
 */
public class {name} {{
    
    private List<Object> data;
    
    /**
     * {description}
     */
    public {name}() {{
        this.data = new ArrayList<>();
    }}
    
    /**
     * Add an item to the collection
     * @param item the item to add
     */
    public void addItem(Object item) {{
        this.data.add(item);
    }}
    
    /**
     * Get all items
     * @return list of all items
     */
    public List<Object> getItems() {{
        return new ArrayList<>(this.data);
    }}
}}'''