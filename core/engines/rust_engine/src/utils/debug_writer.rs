use std::fs::{self, OpenOptions};
use std::io::Write;
use std::path::{Path, PathBuf};
use serde_json::Value;

/// Gestionnaire de logs JSON
pub struct DebugWriter;

impl DebugWriter {
    /// Chemin absolu ou relatif du fichier de logs
    fn log_path() -> PathBuf {
        let dir = Path::new("debug");
        if !dir.exists() {
            if let Err(e) = fs::create_dir_all(dir) {
                eprintln!("Impossible de créer le dossier {:?}: {}", dir, e);
            }
        }
        dir.join("to_fix_rust.json")
    }

    /// Initialise le fichier JSON si nécessaire
    pub fn init() {
        let file_path = Self::log_path();
        if !file_path.exists() {
            if let Err(e) = fs::write(&file_path, "[]") {
                eprintln!("Impossible de créer {:?}: {}", file_path, e);
            }
        }
    }

    /// Log générique avec sérialisation JSON
    pub fn log(message: &str) {
        let file_path = Self::log_path();

        // Lire contenu existant ou créer tableau vide
        let mut logs: Vec<Value> = match fs::read_to_string(&file_path) {
            Ok(content) => serde_json::from_str(&content).unwrap_or_default(),
            Err(_) => Vec::new(),
        };

        logs.push(Value::String(message.to_string()));

        let json = serde_json::to_string_pretty(&logs)
            .unwrap_or_else(|_| format!(r#"["{}"]"#, message));

        // Écrire dans le fichier
        if let Err(e) = OpenOptions::new()
            .write(true)
            .truncate(true)
            .create(true)
            .open(&file_path)
            .and_then(|mut f| f.write_all(json.as_bytes()))
        {
            eprintln!("Impossible d'écrire dans {:?}: {}", file_path, e);
        }

        // Affichage console
        println!("{}", message);
    }

    /// Log de niveau info
    pub fn info(message: &str) {
        Self::log(&format!("[INFO] {}", message));
    }

    /// Log de niveau avertissement
    pub fn warn(message: &str) {
        Self::log(&format!("[WARN] {}", message));
    }

    /// Log de niveau erreur
    pub fn error(message: &str) {
        Self::log(&format!("[ERROR] {}", message));
    }
}

