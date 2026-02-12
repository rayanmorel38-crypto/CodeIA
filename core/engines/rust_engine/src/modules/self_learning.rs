use crate::core::ai_core::{AICore, Role};
use crate::utils::file_ops;
use std::fs;
use std::path::{Path, PathBuf};
use std::io;

/// Gestion du self-learning : analyse et mise à jour des modèles IA
pub struct SelfLearning;

impl SelfLearning {
    /// Initialisation avec les rôles (pour futur usage)
    pub fn init(_roles: &[Role]) {
        println!("[SelfLearning] Initialisation...");
    }

    /// Parcours du dossier modèles et calcul des SHA256
    pub fn update_models() -> Result<usize, io::Error> {
        let model_dir = PathBuf::from("../resources/models/");
        println!("[SelfLearning][INFO] Vérification des modèles dans {:?}", model_dir);

        // Utilisation correcte de &Path
        let files = file_ops::list_files(&model_dir);
        if files.is_empty() {
            println!("[SelfLearning][WARN] Aucun modèle trouvé à mettre à jour.");
            return Ok(0);
        }

        let mut processed = 0;
        for file_path in files {
            let path = Path::new(&file_path);
            if !path.is_file() {
                println!("[SelfLearning][WARN] Ignoré (non fichier) : {:?}", path);
                continue;
            }

            match fs::read(path) {
                Ok(content) => {
                    // SHA256 via AICore, renvoie déjà une String hex
                    let hash = AICore::sha256_bytes(&content);
                    println!(
                        "[SelfLearning][INFO] Modèle {:?} détecté, SHA256: {}",
                        path.file_name().unwrap_or_default(),
                        hash
                    );
                    processed += 1;
                }
                Err(e) => eprintln!("[SelfLearning][ERROR] Impossible de lire {:?}: {}", path, e),
            }
        }

        Ok(processed)
    }
}

