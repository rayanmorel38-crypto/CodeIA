use crate::core::ai_core::{AICore, Role};
use crate::utils::file_ops;
use crate::modules::code_improvement::CodeImprovement;
use std::fs;
use std::path::Path;

/// Raisonnement et analyse logique des fichiers
pub struct Reasoning;

impl Reasoning {
    /// Initialisation avec les rôles (pour futur usage)
    pub fn init(_roles: &[Role]) {
        println!("[Reasoning] Initialisation...");
    }

    /// Analyse complète d'un dossier
    pub fn analyze_disk(dir_path: &Path) {
        println!("[Reasoning][INFO] Analyse du dossier {:?}", dir_path);
        let files = file_ops::list_files(dir_path);
        if files.is_empty() {
            println!("[Reasoning][WARN] Aucun fichier trouvé dans {:?}", dir_path);
            return;
        }

        for file_path in files {
            let path = Path::new(&file_path);
            if !path.is_file() { continue; }

            if let Ok(content) = fs::read(path) {
                // SHA256 via AICore
                let hash = AICore::sha256_bytes(&content);
                println!("[Reasoning][INFO] Fichier {} → SHA256: {}", path.display(), hash);

                // Analyse des fichiers Rust
                if path.extension().and_then(|e| e.to_str()) == Some("rs") {
                    if let Ok(suggestions) = CodeImprovement::analyze_code(path) {
                        if !suggestions.is_empty() {
                            println!("[Reasoning][ALERTE] Problèmes détectés dans {} :", path.display());
                            for s in suggestions { println!("   → {}", s); }
                        } else {
                            println!("[Reasoning][OK] Aucun problème trouvé dans {:?}", path);
                        }
                    }
                }
            } else {
                eprintln!("[Reasoning][ERREUR] Impossible de lire le fichier {}", path.display());
            }
        }
    }

    /// Analyse d'un fichier unique
    pub fn analyze_file(path: &Path) {
        if !path.exists() {
            eprintln!("[Reasoning][ERREUR] Fichier {:?} inexistant.", path);
            return;
        }

        if let Ok(content) = fs::read(path) {
            // SHA256 via AICore
            let hash = AICore::sha256_bytes(&content);
            println!("[Reasoning][INFO] Analyse de {:?}, SHA256: {}", path, hash);

            // Analyse des fichiers Rust
            if path.extension().and_then(|e| e.to_str()) == Some("rs") {
                if let Ok(suggestions) = CodeImprovement::analyze_code(path) {
                    if !suggestions.is_empty() {
                        println!("[Reasoning][ALERTE] Améliorations possibles :");
                        for s in suggestions { println!("   → {}", s); }
                    } else {
                        println!("[Reasoning][OK] Aucun problème détecté.");
                    }
                }
            }
        } else {
            eprintln!("[Reasoning][ERREUR] Impossible de lire {:?}", path);
        }
    }
}

