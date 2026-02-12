use crate::core::ai_core::{AICore, Role};
use std::fs;
use std::path::{Path, PathBuf};
use std::io;

/// Amélioration et correction automatique du code
pub struct CodeImprovement;

impl CodeImprovement {
    /// Analyse le code d’un fichier et retourne les suggestions
    pub fn analyze_code(path: &Path) -> Result<Vec<String>, io::Error> {
        println!("[CodeImprovement][INFO] Analyse de {:?}", path);

        // Utilisation de fs pour lire le fichier
        let content = fs::read_to_string(path)?;
        let mut suggestions = Vec::new();

        if content.contains("unwrap()") {
            suggestions.push("⚠️ Utilisation de unwrap() → remplacer par un match ou expect() plus sûr.".to_string());
        }
        if content.len() > 5000 {
            suggestions.push("⚠️ Fichier volumineux → envisager un découpage en modules.".to_string());
        }

        // Utilisation concrète de AICore pour stocker une analyse temporaire
        let mut core = AICore::new();
        core.set_roles(vec![Role::Analyst]);
        core.integrate_code(format!("Analyse du fichier {:?}, {} suggestions", path, suggestions.len()));
        println!("[CodeImprovement][INFO] Analyse intégrée dans AICore :\n{}", core.generate_code());

        Ok(suggestions)
    }

    /// Corrige automatiquement certains problèmes simples dans le fichier
    pub fn auto_fix(path: &Path) -> Result<(), io::Error> {
        println!("[CodeImprovement][INFO] Tentative de correction automatique de {:?}", path);

        let mut content = fs::read_to_string(path)?;
        content = content.replace("unwrap()", "expect(\"Checked unwrap\")");

        // Utilisation de PathBuf pour construire un nouveau chemin de sauvegarde
        let mut backup_path = PathBuf::from(path);
        backup_path.set_extension("bak");
        fs::write(&backup_path, fs::read(path)?)?;
        fs::write(path, content)?;

        println!("[CodeImprovement][INFO] Corrections appliquées à {:?}, sauvegarde créée : {:?}", path, backup_path);
        Ok(())
    }

    /// Intègre les retours utilisateurs pour ajuster le code
    pub fn integrate_user_feedback(path: &Path, feedback: &[String]) -> Result<(), io::Error> {
        println!("[CodeImprovement][INFO] Intégration du feedback utilisateur pour {:?}", path);

        for f in feedback {
            println!("   → Feedback intégré : {}", f);
        }

        // Utilisation de AICore pour stocker le feedback intégré
        let mut core = AICore::new();
        core.set_roles(vec![Role::Developer, Role::Analyst]);
        core.integrate_code(format!("Feedback sur {:?} : {} éléments", path, feedback.len()));
        println!("[CodeImprovement][INFO] Feedback intégré dans AICore :\n{}", core.generate_code());

        Ok(())
    }
}

