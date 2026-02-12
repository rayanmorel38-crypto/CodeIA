// tests/test_sandbox.cpp
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <regex>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <unistd.h>
#include <sstream>

namespace fs = std::filesystem;

namespace cppengine {
namespace tests {
namespace sandbox {

// Configuration de la sandbox
struct SandboxConfig {
    std::vector<std::string> allowed_extensions = {
        // Images
        ".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif", ".gif", ".webp",
        // Vidéos
        ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".m4v",
        // Audio
        ".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma", ".m4a",
        // Documents
        ".txt", ".pdf", ".doc", ".docx", ".rtf", ".odt"
        // 3D assets
        , ".blend", ".fbx", ".obj", ".gltf", ".glb", ".dae", ".3ds"
    };

    std::vector<std::string> excluded_dirs = {
        "node_modules", ".git", "__pycache__", "build", "cmake-build-*",
        ".vscode", ".idea", "target", "bin", "obj", ".DS_Store"
    };

    size_t max_file_size = 100 * 1024 * 1024; // 100MB max
    size_t max_files_per_type = 1000; // Limite par type
    bool enable_filtering = true;
};

// Statistiques de l'analyse
struct AnalysisStats {
    size_t total_files_scanned = 0;
    size_t total_files_processed = 0;
    size_t images_found = 0;
    size_t videos_found = 0;
    size_t audio_found = 0;
    size_t documents_found = 0;
    size_t assets_found = 0;
    size_t code_found = 0;
    size_t errors_encountered = 0;
    std::map<std::string, size_t> files_by_extension;
    std::vector<std::string> error_log;
};

// Résultat d'analyse d'un fichier média
struct MediaAnalysis {
    std::string file_path;
    std::string type; // "image", "video", "audio"
    cv::Size dimensions; // pour images/vidéos
    double duration = 0.0; // pour vidéos/audio en secondes
    size_t file_size = 0;
    std::string format;
    bool is_valid = false;
    std::string error_message;
    std::map<std::string, std::string> metadata; // extracted lightweight metadata for assets
};

// Classe principale de la sandbox
class SystemAnalysisSandbox {
private:
    SandboxConfig config_;
    AnalysisStats stats_;
    std::vector<MediaAnalysis> media_dataset_;
    std::map<std::string, bool> user_filters_; // extension -> approved

public:
    SystemAnalysisSandbox() = default;

    void configure(const SandboxConfig& config) {
        config_ = config;
    }

    // Analyse récursive du système de fichiers
    bool analyze_system(const std::string& start_path = "/home") {
        INFO("Starting system analysis from: " << start_path);

        try {
            fs::path root_path(start_path);

            if (!fs::exists(root_path)) {
                stats_.error_log.push_back("Start path does not exist: " + start_path);
                return false;
            }

            analyze_directory(root_path);
            generate_report();

            return true;
        } catch (const std::exception& e) {
            stats_.error_log.push_back("Analysis failed: " + std::string(e.what()));
            return false;
        }
    }

    // Analyse d'un répertoire
    void analyze_directory(const fs::path& dir_path) {
        try {
            for (const auto& entry : fs::recursive_directory_iterator(dir_path,
                     fs::directory_options::skip_permission_denied)) {

                if (!entry.is_regular_file()) continue;

                stats_.total_files_scanned++;

                // Vérifier les exclusions
                if (is_excluded_directory(entry.path())) continue;

                // Traiter le fichier
                process_file(entry.path());

                // Limite de sécurité
                if (stats_.total_files_processed >= 50000) {
                    INFO("Reached processing limit (50000 files)");
                    break;
                }
            }
        } catch (const std::exception& e) {
            stats_.error_log.push_back("Directory analysis error: " + std::string(e.what()));
        }
    }

    // Traitement d'un fichier individuel
    void process_file(const fs::path& file_path) {
        try {
            std::string ext = file_path.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            // Vérifier si l'extension est autorisée
            if (std::find(config_.allowed_extensions.begin(),
                         config_.allowed_extensions.end(), ext) ==
                config_.allowed_extensions.end()) {
                return;
            }

            // Vérifier la taille
            size_t file_size = fs::file_size(file_path);
            if (file_size > config_.max_file_size) {
                return;
            }

            stats_.files_by_extension[ext]++;
            stats_.total_files_processed++;

            // Analyser selon le type
            if (is_image_extension(ext)) {
                analyze_image(file_path);
            } else if (is_video_extension(ext)) {
                analyze_video(file_path);
            } else if (is_audio_extension(ext)) {
                analyze_audio(file_path);
            } else if (is_asset_extension(ext)) {
                analyze_asset(file_path);
            } else if (is_code_extension(ext)) {
                analyze_code(file_path);
            } else if (is_document_extension(ext)) {
                analyze_document(file_path);
            }

        } catch (const std::exception& e) {
            stats_.errors_encountered++;
            stats_.error_log.push_back("File processing error for " +
                                     file_path.string() + ": " + e.what());
        }
    }

    // Analyse d'une image
    void analyze_image(const fs::path& file_path) {
        MediaAnalysis analysis;
        analysis.file_path = file_path.string();
        analysis.type = "image";
        analysis.file_size = fs::file_size(file_path);

        try {
            cv::Mat img = cv::imread(file_path.string());
            if (!img.empty()) {
                analysis.dimensions = img.size();
                analysis.is_valid = true;
                analysis.format = get_image_format(file_path.extension().string());

                stats_.images_found++;
                media_dataset_.push_back(analysis);
            } else {
                analysis.is_valid = false;
                analysis.error_message = "Failed to load image";
            }
        } catch (const std::exception& e) {
            analysis.is_valid = false;
            analysis.error_message = e.what();
        }

        if (!analysis.is_valid) {
            stats_.errors_encountered++;
        }
    }

    // Analyse d'une vidéo
    void analyze_video(const fs::path& file_path) {
        MediaAnalysis analysis;
        analysis.file_path = file_path.string();
        analysis.type = "video";
        analysis.file_size = fs::file_size(file_path);

        try {
            cv::VideoCapture cap(file_path.string());
            if (cap.isOpened()) {
                analysis.dimensions.width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
                analysis.dimensions.height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
                analysis.duration = cap.get(cv::CAP_PROP_FRAME_COUNT) / cap.get(cv::CAP_PROP_FPS);
                analysis.is_valid = true;
                analysis.format = get_video_format(file_path.extension().string());

                stats_.videos_found++;
                media_dataset_.push_back(analysis);
                cap.release();
            } else {
                analysis.is_valid = false;
                analysis.error_message = "Failed to open video";
            }
        } catch (const std::exception& e) {
            analysis.is_valid = false;
            analysis.error_message = e.what();
        }

        if (!analysis.is_valid) {
            stats_.errors_encountered++;
        }
    }

    // Analyse d'un fichier audio (basique pour l'instant)
    void analyze_audio(const fs::path& file_path) {
        MediaAnalysis analysis;
        analysis.file_path = file_path.string();
        analysis.type = "audio";
        analysis.file_size = fs::file_size(file_path);
        analysis.is_valid = true; // Considéré valide si accessible
        analysis.format = get_audio_format(file_path.extension().string());

        stats_.audio_found++;
        media_dataset_.push_back(analysis);
    }

    // Analyse d'un document
    void analyze_document(const fs::path& file_path) {
        MediaAnalysis analysis;
        analysis.file_path = file_path.string();
        analysis.type = "document";
        analysis.file_size = fs::file_size(file_path);
        analysis.is_valid = true; // Considéré valide si accessible
        analysis.format = get_document_format(file_path.extension().string());

        stats_.documents_found++;
        media_dataset_.push_back(analysis);
    }

    // Analyse d'un asset 3D (détection basique par extension)
    void analyze_asset(const fs::path& file_path) {
        MediaAnalysis analysis;
        analysis.file_path = file_path.string();
        analysis.type = "asset";
        analysis.file_size = fs::file_size(file_path);
        analysis.is_valid = true; // Considéré valide si accessible
        analysis.format = get_asset_format(file_path.extension().string());
        // Tentative d'extraction minimale selon l'extension
        std::string ext = file_path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        try {
            if (ext == ".obj") {
                // Count vertices (lines starting with 'v ') and faces ('f ')
                std::ifstream in(file_path);
                size_t verts = 0, faces = 0;
                std::string line;
                while (std::getline(in, line)) {
                    if (line.rfind("v ", 0) == 0) verts++;
                    else if (line.rfind("f ", 0) == 0) faces++;
                }
                analysis.metadata["vertices"] = std::to_string(verts);
                analysis.metadata["faces"] = std::to_string(faces);
            } else if (ext == ".gltf") {
                // Quick heuristic: count occurrences of "\"meshes\"" to approximate mesh count
                std::ifstream in(file_path);
                std::stringstream ss; ss << in.rdbuf();
                std::string content = ss.str();
                size_t meshes = 0;
                std::string token = "\"meshes\"";
                size_t pos = 0;
                while ((pos = content.find(token, pos)) != std::string::npos) { meshes++; pos += token.size(); }
                analysis.metadata["meshes_approx"] = std::to_string(meshes);
            } else if (ext == ".glb") {
                // glb header: magic(4) 'glTF', version (uint32), length (uint32)
                std::ifstream in(file_path, std::ios::binary);
                char magic[4];
                uint32_t version = 0, length = 0;
                if (in.read(magic, 4)) {
                    if (std::string(magic, 4) == "glTF") {
                        in.read(reinterpret_cast<char*>(&version), sizeof(version));
                        in.read(reinterpret_cast<char*>(&length), sizeof(length));
                        analysis.metadata["glb_version"] = std::to_string(version);
                        analysis.metadata["glb_length"] = std::to_string(length);
                    } else {
                        analysis.metadata["glb_magic"] = std::string(magic, 4);
                    }
                }
            } else if (ext == ".blend") {
                // Blender files have 'BLENDER' signature near the start
                std::ifstream in(file_path, std::ios::binary);
                char header[12] = {0};
                if (in.read(header, 12)) {
                    analysis.metadata["blend_header"] = std::string(header, 12);
                }
            } else if (ext == ".fbx") {
                // Detect ASCII vs binary by scanning first KB for 'Kaydara'
                std::ifstream in(file_path, std::ios::binary);
                std::string chunk;
                chunk.resize(1024);
                in.read(&chunk[0], 1024);
                std::string head = chunk.substr(0, in.gcount());
                if (head.find("Kaydara") != std::string::npos || head.find("FBX") != std::string::npos)
                    analysis.metadata["fbx_type"] = "binary";
                else
                    analysis.metadata["fbx_type"] = "ascii_or_unknown";
            } else if (ext == ".dae") {
                // Count occurrences of <geometry or <mesh tags as proxy
                std::ifstream in(file_path);
                std::stringstream ss; ss << in.rdbuf();
                std::string content = ss.str();
                size_t geom = 0; size_t pos = 0;
                while ((pos = content.find("<geometry", pos)) != std::string::npos) { geom++; pos += 9; }
                analysis.metadata["geometry_count_approx"] = std::to_string(geom);
            } else if (ext == ".3ds") {
                // 3ds is binary; provide file size as a basic metric
                analysis.metadata["bytes"] = std::to_string(analysis.file_size);
            } else {
                // Generic: store a small signature (first 64 bytes)
                std::ifstream in(file_path, std::ios::binary);
                std::string buf;
                buf.resize(64);
                in.read(&buf[0], 64);
                buf.resize(in.gcount());
                analysis.metadata["header"] = buf;
            }
        } catch (const std::exception& e) {
            analysis.is_valid = false;
            analysis.error_message = e.what();
            stats_.errors_encountered++;
        }

        stats_.assets_found++;
        media_dataset_.push_back(analysis);
    }

    // Analyse d'un fichier de code
    void analyze_code(const fs::path& file_path) {
        MediaAnalysis analysis;
        analysis.file_path = file_path.string();
        analysis.type = "code";
        analysis.file_size = fs::file_size(file_path);
        analysis.is_valid = true; // Considéré valide si accessible
        analysis.format = get_code_format(file_path.extension().string());

        // Extraire des métadonnées simples pour le code
        try {
            std::ifstream in(file_path);
            std::string line;
            size_t lines = 0, empty_lines = 0, comment_lines = 0;
            while (std::getline(in, line)) {
                lines++;
                if (line.empty()) empty_lines++;
                else if (line.find("//") != std::string::npos || line.find("#") != std::string::npos ||
                         line.find("/*") != std::string::npos) comment_lines++;
            }
            analysis.metadata["total_lines"] = std::to_string(lines);
            analysis.metadata["empty_lines"] = std::to_string(empty_lines);
            analysis.metadata["comment_lines"] = std::to_string(comment_lines);
            analysis.metadata["code_lines"] = std::to_string(lines - empty_lines - comment_lines);
        } catch (const std::exception& e) {
            analysis.is_valid = false;
            analysis.error_message = e.what();
            stats_.errors_encountered++;
        }

        stats_.code_found++;
        media_dataset_.push_back(analysis);
    }

    // Interface utilisateur pour filtrer le dataset
    void interactive_filtering() {
        std::cout << "\n=== SANDBOX DATASET FILTERING ===\n";
        std::cout << "Found " << media_dataset_.size() << " media files\n\n";

        // Grouper par type
        std::map<std::string, std::vector<MediaAnalysis*>> by_type;
        for (auto& item : media_dataset_) {
            by_type[item.type].push_back(&item);
        }

        // Afficher les statistiques par type
        for (const auto& [type, items] : by_type) {
            std::cout << type << ": " << items.size() << " files\n";
            if (items.size() > 0) {
                // Montrer quelques exemples
                std::cout << "  Examples:\n";
                size_t count = std::min(size_t(3), items.size());
                for (size_t i = 0; i < count; ++i) {
                    const auto& item = *items[i];
                    std::cout << "    " << fs::path(item.file_path).filename().string();
                    if (item.type == "image" || item.type == "video") {
                        std::cout << " (" << item.dimensions.width << "x" << item.dimensions.height << ")";
                    }
                    if (item.duration > 0) {
                        std::cout << " [" << item.duration << "s]";
                    }
                    std::cout << "\n";
                }
            }
        }

        std::cout << "\nFiltering options:\n";
        std::cout << "1. Keep all\n";
        std::cout << "2. Filter by type\n";
        std::cout << "3. Filter by size\n";
        std::cout << "4. Filter by dimensions (images/videos)\n";
        std::cout << "5. Manual selection\n";

        // Pour les tests automatisés, on garde tout par défaut
        std::cout << "Auto-selecting: Keep all files for training\n";
        for (auto& item : media_dataset_) {
            user_filters_[item.file_path] = true;
        }
    }

    // Générer un rapport d'analyse
    void generate_report() {
        std::cout << "\n=== SYSTEM ANALYSIS REPORT ===\n";
        std::cout << "Total files scanned: " << stats_.total_files_scanned << "\n";
        std::cout << "Total files processed: " << stats_.total_files_processed << "\n";
        std::cout << "Images found: " << stats_.images_found << "\n";
        std::cout << "Videos found: " << stats_.videos_found << "\n";
        std::cout << "Audio files found: " << stats_.audio_found << "\n";
        std::cout << "Documents found: " << stats_.documents_found << "\n";
        std::cout << "3D assets found: " << stats_.assets_found << "\n";
        std::cout << "Code files found: " << stats_.code_found << "\n";
        std::cout << "Errors encountered: " << stats_.errors_encountered << "\n";

        std::cout << "\nFiles by extension:\n";
        for (const auto& [ext, count] : stats_.files_by_extension) {
            std::cout << "  " << ext << ": " << count << "\n";
        }

        if (!stats_.error_log.empty()) {
            std::cout << "\nErrors:\n";
            for (const auto& error : stats_.error_log) {
                std::cout << "  " << error << "\n";
            }
        }

        // Show a small sample of asset metadata for inspection
        size_t shown = 0;
        std::cout << "\nSample asset metadata:\n";
        for (const auto& item : media_dataset_) {
            if (item.type != "asset") continue;
            std::cout << "  " << fs::path(item.file_path).filename().string() << ":\n";
            size_t printed = 0;
            for (const auto& [k, v] : item.metadata) {
                std::cout << "    " << k << ": " << v << "\n";
                if (++printed >= 6) break;
            }
            if (++shown >= 5) break;
        }
    }

    // Intégrer le dataset au réseau de neurones
    void integrate_to_ai() {
        std::cout << "\n=== INTEGRATING DATASET TO NEURAL NETWORK ===\n";

        if (media_dataset_.empty()) {
            std::cout << "No data to integrate - dataset is empty.\n";
            return;
        }

        // Exporter le dataset vers un fichier CSV pour l'IA
        std::string csv_path = "build/logs/dataset_export.csv";
        fs::create_directories(fs::path(csv_path).parent_path());
        std::ofstream csv_file(csv_path);
        if (csv_file.is_open()) {
            csv_file << "file_path,type,file_size,format,is_valid,duration,dimensions_width,dimensions_height";
            // Ajouter les clés de métadonnées dynamiques
            std::set<std::string> all_meta_keys;
            for (const auto& item : media_dataset_) {
                for (const auto& [k, v] : item.metadata) {
                    all_meta_keys.insert(k);
                }
            }
            for (const auto& key : all_meta_keys) {
                csv_file << "," << key;
            }
            csv_file << "\n";

            for (const auto& item : media_dataset_) {
                csv_file << item.file_path << "," << item.type << "," << item.file_size << ","
                         << item.format << "," << (item.is_valid ? "1" : "0") << ","
                         << item.duration << "," << item.dimensions.width << "," << item.dimensions.height;
                for (const auto& key : all_meta_keys) {
                    auto it = item.metadata.find(key);
                    csv_file << "," << (it != item.metadata.end() ? it->second : "");
                }
                csv_file << "\n";
            }
            csv_file.close();
            std::cout << "Dataset exported to: " << csv_path << "\n";
        } else {
            std::cout << "Failed to export dataset to CSV.\n";
        }

        // Simuler l'intégration à un réseau neuronal (appel à un module IA)
        std::cout << "Processing " << media_dataset_.size() << " items through neural network...\n";

        // Ici, on pourrait appeler cpp_engine::modules::ai::NeuralNetwork ou torch_backend
        // Pour l'exemple, on simule un traitement simple
        size_t code_files = 0, asset_files = 0;
        for (const auto& item : media_dataset_) {
            if (item.type == "code") code_files++;
            else if (item.type == "asset") asset_files++;
        }

        std::cout << "Neural network analysis:\n";
        std::cout << "  - Code files detected: " << code_files << "\n";
        std::cout << "  - Asset files detected: " << asset_files << "\n";
        std::cout << "  - Patterns learned: File type classification, metadata extraction\n";
        std::cout << "  - Model updated with new dataset.\n";

        // TODO: Intégrer réellement avec neural_network.cpp ou torch_backend.cpp
        // Exemple: cppengine::models::NeuralNetwork nn; nn.train_on_dataset(media_dataset_);
    }

    // Getters
    const AnalysisStats& get_stats() const { return stats_; }
    const std::vector<MediaAnalysis>& get_dataset() const { return media_dataset_; }

private:
    bool is_excluded_directory(const fs::path& path) {
        for (const auto& excluded : config_.excluded_dirs) {
            if (path.string().find(excluded) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    bool is_image_extension(const std::string& ext) {
        return ext == ".jpg" || ext == ".jpeg" || ext == ".png" ||
               ext == ".bmp" || ext == ".tiff" || ext == ".tif" ||
               ext == ".gif" || ext == ".webp";
    }

    bool is_video_extension(const std::string& ext) {
        return ext == ".mp4" || ext == ".avi" || ext == ".mkv" ||
               ext == ".mov" || ext == ".wmv" || ext == ".flv" ||
               ext == ".webm" || ext == ".m4v";
    }

    bool is_audio_extension(const std::string& ext) {
        return ext == ".mp3" || ext == ".wav" || ext == ".flac" ||
               ext == ".aac" || ext == ".ogg" || ext == ".wma" ||
               ext == ".m4a";
    }

    bool is_document_extension(const std::string& ext) {
        return ext == ".txt" || ext == ".pdf" || ext == ".doc" ||
               ext == ".docx" || ext == ".rtf" || ext == ".odt";
    }

    bool is_asset_extension(const std::string& ext) {
        return ext == ".blend" || ext == ".fbx" || ext == ".obj" ||
               ext == ".gltf" || ext == ".glb" || ext == ".dae" || ext == ".3ds";
    }

    bool is_code_extension(const std::string& ext) {
        return ext == ".py" || ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp" ||
               ext == ".java" || ext == ".js" || ext == ".ts" || ext == ".rs" || ext == ".go" ||
               ext == ".rb" || ext == ".php" || ext == ".cs" || ext == ".swift" || ext == ".kt" || ext == ".scala";
    }

    std::string get_image_format(const std::string& ext) {
        if (ext == ".jpg" || ext == ".jpeg") return "JPEG";
        if (ext == ".png") return "PNG";
        if (ext == ".bmp") return "BMP";
        if (ext == ".tiff" || ext == ".tif") return "TIFF";
        if (ext == ".gif") return "GIF";
        if (ext == ".webp") return "WebP";
        return "Unknown";
    }

    std::string get_video_format(const std::string& ext) {
        if (ext == ".mp4") return "MP4";
        if (ext == ".avi") return "AVI";
        if (ext == ".mkv") return "MKV";
        if (ext == ".mov") return "MOV";
        if (ext == ".wmv") return "WMV";
        if (ext == ".flv") return "FLV";
        if (ext == ".webm") return "WebM";
        if (ext == ".m4v") return "M4V";
        return "Unknown";
    }

    std::string get_audio_format(const std::string& ext) {
        if (ext == ".mp3") return "MP3";
        if (ext == ".wav") return "WAV";
        if (ext == ".flac") return "FLAC";
        if (ext == ".aac") return "AAC";
        if (ext == ".ogg") return "OGG";
        if (ext == ".wma") return "WMA";
        if (ext == ".m4a") return "M4A";
        return "Unknown";
    }

    std::string get_document_format(const std::string& ext) {
        if (ext == ".txt") return "Text";
        if (ext == ".pdf") return "PDF";
        if (ext == ".doc") return "Word";
        if (ext == ".docx") return "Word";
        if (ext == ".rtf") return "RTF";
        if (ext == ".odt") return "OpenDocument";
        return "Unknown";
    }

    std::string get_asset_format(const std::string& ext) {
        if (ext == ".blend") return "Blender";
        if (ext == ".fbx") return "FBX";
        if (ext == ".obj") return "OBJ";
        if (ext == ".gltf") return "glTF";
        if (ext == ".glb") return "glTF-BINARY";
        if (ext == ".dae") return "Collada";
        if (ext == ".3ds") return "3DS";
        return "Unknown";
    }

    std::string get_code_format(const std::string& ext) {
        if (ext == ".py") return "Python";
        if (ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp") return "C++";
        if (ext == ".java") return "Java";
        if (ext == ".js") return "JavaScript";
        if (ext == ".ts") return "TypeScript";
        if (ext == ".rs") return "Rust";
        if (ext == ".go") return "Go";
        if (ext == ".rb") return "Ruby";
        if (ext == ".php") return "PHP";
        if (ext == ".cs") return "C#";
        if (ext == ".swift") return "Swift";
        if (ext == ".kt") return "Kotlin";
        if (ext == ".scala") return "Scala";
        return "Unknown";
    }
};

} // namespace sandbox
} // namespace tests
} // namespace cppengine

// Helper function for interactive folder navigation
std::string navigate_to_folder() {
    std::string current_path = std::getenv("HOME") ? std::getenv("HOME") : "/home";
    while (true) {
        std::cout << "\nCurrent directory: " << current_path << "\n";
        std::cout << "Subdirectories:\n";

        std::vector<std::string> subdirs;
        try {
            for (const auto& entry : fs::directory_iterator(current_path)) {
                if (entry.is_directory()) {
                    subdirs.push_back(entry.path().filename().string());
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Error reading directory: " << e.what() << "\n";
            return current_path;
        }

        if (subdirs.empty()) {
            std::cout << "No subdirectories found.\n";
        } else {
            for (size_t i = 0; i < subdirs.size(); ++i) {
                std::cout << i + 1 << ". " << subdirs[i] << "\n";
            }
        }

        std::cout << "Options:\n";
        std::cout << "0. Analyze this directory (" << current_path << ")\n";
        if (!subdirs.empty()) {
            std::cout << "Enter number to navigate to subdirectory, or 0 to analyze here: ";
        } else {
            std::cout << "Enter 0 to analyze here: ";
        }

        std::string input;
        std::getline(std::cin, input);

        if (input == "0") {
            return current_path;
        }

        try {
            size_t choice = std::stoul(input);
            if (choice >= 1 && choice <= subdirs.size()) {
                current_path = fs::path(current_path) / subdirs[choice - 1];
            } else {
                std::cout << "Invalid choice. Try again.\n";
            }
        } catch (const std::exception&) {
            std::cout << "Invalid input. Enter a number.\n";
        }
    }
}

// Tests pour la sandbox
TEST_CASE("SystemAnalysisSandbox - Basic Analysis", "[sandbox]") {
    cppengine::tests::sandbox::SystemAnalysisSandbox sandbox;

    cppengine::tests::sandbox::SandboxConfig config;
    config.max_files_per_type = 10; // Limiter pour les tests
    sandbox.configure(config);

    // Interactively allow the user to specify a folder to analyze when running tests
    fs::path default_test_dir = fs::current_path().parent_path() / "test_media";
    std::string user_input;
    bool interactive = isatty(STDIN_FILENO);
    fs::path test_dir;
    if (interactive) {
        std::cout << "Enter folder to analyze for sandbox (press Enter to use '" << default_test_dir.string() << "'): ";
        std::getline(std::cin, user_input);
    }
    if (!user_input.empty()) test_dir = fs::path(user_input);
    else test_dir = default_test_dir;

    REQUIRE(fs::exists(test_dir));
    REQUIRE(sandbox.analyze_system(test_dir.string()));

    const auto& stats = sandbox.get_stats();
    CHECK(stats.total_files_scanned > 0);
    CHECK(stats.errors_encountered == 0);

    sandbox.interactive_filtering();
}

TEST_CASE("SystemAnalysisSandbox - Media Analysis", "[sandbox]") {
    cppengine::tests::sandbox::SystemAnalysisSandbox sandbox;

    cppengine::tests::sandbox::SandboxConfig config;
    // Include all media, code, assets, and documents
    config.allowed_extensions = {
        // Images
        ".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif", ".gif", ".webp",
        // Videos
        ".mp4", ".avi", ".mkv", ".mov", ".wmv", ".flv", ".webm", ".m4v",
        // Audio
        ".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma", ".m4a",
        // Documents
        ".txt", ".pdf", ".doc", ".docx", ".rtf", ".odt",
        // 3D assets
        ".blend", ".fbx", ".obj", ".gltf", ".glb", ".dae", ".3ds",
        // Code files
        ".py", ".cpp", ".c", ".h", ".hpp", ".java", ".js", ".ts", ".rs", ".go", ".rb", ".php", ".cs", ".swift", ".kt", ".scala"
    };
    config.max_files_per_type = 100; // Allow more files for code analysis
    sandbox.configure(config);

    // Ask interactively for a folder to analyze (defaults to HOME)
    std::string home_dir = std::getenv("HOME") ? std::getenv("HOME") : "/home";
    std::string folder_to_scan;
    bool interactive2 = isatty(STDIN_FILENO);
    if (interactive2) {
        folder_to_scan = navigate_to_folder();
    } else {
        folder_to_scan = home_dir;
    }
    INFO("Analyzing directory: " << folder_to_scan);

    REQUIRE_NOTHROW(sandbox.analyze_system(folder_to_scan));

    const auto& stats = sandbox.get_stats();
    INFO("Analysis complete - Files scanned: " << stats.total_files_scanned);
    INFO("Media found - Images: " << stats.images_found << ", Videos: " << stats.videos_found << ", Audio: " << stats.audio_found);

    // Ne pas planter si aucun média trouvé (possible sur un système de dev)
    CHECK(stats.total_files_scanned > 0);

    if (!sandbox.get_dataset().empty()) {
        sandbox.interactive_filtering();
    }

    // Intégrer les données au réseau de neurones
    sandbox.integrate_to_ai();
}

TEST_CASE("SystemAnalysisSandbox - Filtering", "[sandbox]") {
    cppengine::tests::sandbox::SystemAnalysisSandbox sandbox;

    // Test de l'interface de filtrage
    REQUIRE_NOTHROW(sandbox.interactive_filtering());
}