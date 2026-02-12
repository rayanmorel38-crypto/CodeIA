use std::fs::{self, File};
use std::io::{self, Read, Write};
use std::path::Path;

/// Lit le contenu d’un fichier en texte
pub fn read_file(path: &Path) -> io::Result<String> {
    let mut file = File::open(path)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    Ok(contents)
}

/// Écrit du texte dans un fichier
pub fn write_file(path: &Path, content: &str) -> io::Result<()> {
    if let Some(parent) = path.parent() {
        // on crée le dossier parent si nécessaire
        if !parent.exists() {
            fs::create_dir_all(parent)?;
        }
    }
    let mut file = File::create(path)?;
    file.write_all(content.as_bytes())?;
    Ok(())
}

/// Vérifie si un fichier existe
pub fn file_exists(path: &Path) -> bool {
    path.exists()
}

/// Crée un dossier si inexistant
pub fn create_dir_if_missing(path: &Path) -> io::Result<()> {
    if !path.exists() {
        fs::create_dir_all(path)?;
    }
    Ok(())
}

/// Liste tous les fichiers dans un dossier de manière récursive
pub fn list_files(dir: &Path) -> Vec<String> {
    let mut files = Vec::new();
    if dir.is_dir() {
        if let Ok(entries) = fs::read_dir(dir) {
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_file() {
                    files.push(path.to_string_lossy().into_owned());
                } else if path.is_dir() {
                    files.extend(list_files(&path));
                }
            }
        }
    }
    files
}

/// Récupère le chemin absolu d’un fichier
pub fn absolute_path(path: &Path) -> io::Result<String> {
    Ok(fs::canonicalize(path)?.to_string_lossy().into_owned())
}

