/// Convertit une chaîne en snake_case
pub fn to_snake_case(s: &str) -> String {
    s.chars()
        .flat_map(|c| {
            if c.is_uppercase() {
                vec!['_', c.to_ascii_lowercase()]
            } else {
                vec![c]
            }
        })
        .collect::<String>()
        .trim_start_matches('_')
        .to_string()
}

/// Convertit une chaîne en camelCase
pub fn to_camel_case(s: &str) -> String {
    let mut result = String::new();
    let mut upper_next = false;
    for c in s.chars() {
        if c == '_' || c == ' ' {
            upper_next = true;
        } else if upper_next {
            result.push(c.to_ascii_uppercase());
            upper_next = false;
        } else {
            result.push(c);
        }
    }
    result
}

/// Vérifie si une chaîne contient uniquement des chiffres
pub fn is_numeric(s: &str) -> bool {
    !s.is_empty() && s.chars().all(|c| c.is_ascii_digit())
}

/// Vérifie si une chaîne est vide ou ne contient que des espaces
pub fn is_blank(s: &str) -> bool {
    s.trim().is_empty()
}

