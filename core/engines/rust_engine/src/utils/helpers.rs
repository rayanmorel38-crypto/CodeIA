/// Vérifie si une chaîne est vide ou ne contient que des espaces
pub fn is_blank(s: &str) -> bool {
    s.trim().is_empty()
}

/// Retourne la valeur minimum d’un slice de nombres
pub fn min<T: Ord + Copy>(slice: &[T]) -> Option<T> {
    slice.iter().copied().min()
}

/// Retourne la valeur maximum d’un slice de nombres
pub fn max<T: Ord + Copy>(slice: &[T]) -> Option<T> {
    slice.iter().copied().max()
}

/// Fonction utilitaire pour répéter un bloc plusieurs fois
pub fn repeat<F: FnMut()>(times: usize, mut f: F) {
    for _ in 0..times {
        f();
    }
}

