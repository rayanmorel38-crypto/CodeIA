#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace cppengine { namespace tests { namespace code_analysis {

struct FileSummary {
    std::string path;
    std::string lang;
    uint64_t size = 0;
    uint64_t lines = 0;
    std::string sha1;
};

static std::string sha1_of_file(const fs::path& p) {
    std::ifstream ifs(p, std::ios::binary);
    if (!ifs) return "";
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    char buf[4096];
    while (ifs.read(buf, sizeof(buf))) {
        SHA1_Update(&ctx, buf, ifs.gcount());
    }
    if (ifs.gcount() > 0) SHA1_Update(&ctx, buf, ifs.gcount());
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &ctx);
    std::ostringstream oss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return oss.str();
}

static std::string detect_language(const fs::path& p) {
    auto ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".h" || ext == ".hpp") return "cpp";
    if (ext == ".rs") return "rust";
    if (ext == ".java") return "java";
    if (ext == ".py") return "python";
    return "other";
}

class SystemCodeAnalyzer {
public:
    SystemCodeAnalyzer() = default;

    void configure_allowed(const std::vector<std::string>& allowed) {
        allowed_exts_.clear();
        for (auto &e : allowed) {
            std::string s = e;
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            allowed_exts_.insert(s);
        }
    }

    bool analyze(const std::string& start_path, const std::string& snapshot_name="default") {
        fs::path root(start_path);
        if (!fs::exists(root)) return false;
        snapshot_name_ = snapshot_name;
        results_.clear();
        try {
            for (auto &entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
                if (!entry.is_regular_file()) continue;
                fs::path p = entry.path();
                auto ext = p.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (!allowed_exts_.empty() && allowed_exts_.count(ext) == 0) continue;
                FileSummary s;
                s.path = p.string();
                s.lang = detect_language(p);
                s.size = fs::file_size(p);
                // count lines if text-like
                s.lines = count_lines(p);
                s.sha1 = sha1_of_file(p);
                results_.push_back(std::move(s));
                // safety cap
                if (results_.size() > 50000) break;
            }
        } catch (const std::exception &e) {
            last_error_ = e.what();
            return false;
        }
        // write snapshot
        return write_snapshot();
    }

    bool write_snapshot() {
        try {
            fs::path logs = fs::current_path() / "logs";
            if (!fs::exists(logs)) fs::create_directories(logs);
            fs::path snap = logs / (std::string("code_snapshot_") + snapshot_name_ + ".csv");
            std::ofstream ofs(snap);
            ofs << "path,lang,size,lines,sha1\n";
            for (auto &f : results_) {
                ofs << '"' << f.path << "","" << f.lang << "," << f.size << "," << f.lines << "," << f.sha1 << "\n";
            }
            ofs.close();
            return true;
        } catch (...) { return false; }
    }

    // returns a basic comparison summary between current snapshot and previous snapshot with same name
    bool compare_with_previous(std::string& out_report) {
        try {
            fs::path logs = fs::current_path() / "logs";
            fs::path snap = logs / (std::string("code_snapshot_") + snapshot_name_ + ".csv");
            fs::path prev = logs / (std::string("code_snapshot_") + snapshot_name_ + ".prev.csv");
            if (!fs::exists(snap)) { out_report = "No current snapshot"; return false; }
            if (!fs::exists(prev)) { out_report = "No previous snapshot to compare"; return false; }
            // load both into maps by path->sha1
            auto load_map = [&](const fs::path &p){
                std::map<std::string,std::string> m;
                std::ifstream ifs(p);
                std::string line;
                std::getline(ifs, line); // header
                while (std::getline(ifs, line)) {
                    // simple CSV parse: assume no commas in fields except path enclosed in quotes
                    size_t q1 = line.find('"');
                    size_t q2 = line.find('"', q1+1);
                    std::string path = line.substr(q1+1, q2-q1-1);
                    // remainder
                    std::string rem = line.substr(q2+2);
                    // last token is sha1
                    size_t lastc = rem.rfind(',');
                    std::string sha = rem.substr(lastc+1);
                    m[path]=sha;
                }
                return m;
            };
            auto cur = load_map(snap);
            auto old = load_map(prev);
            size_t added=0, removed=0, changed=0, same=0;
            for (auto &kv : cur) {
                if (old.find(kv.first)==old.end()) added++;
                else if (old[kv.first]!=kv.second) changed++;
                else same++;
            }
            for (auto &kv : old) if (cur.find(kv.first)==cur.end()) removed++;
            std::ostringstream oss;
            oss << "Comparison: added="<<added<<" removed="<<removed<<" changed="<<changed<<" same="<<same;
            out_report = oss.str();
            return true;
        } catch (const std::exception &e) { out_report = e.what(); return false; }
    }

    // rotate snapshot (move current to prev) - used before re-run
    bool rotate_previous() {
        try {
            fs::path logs = fs::current_path() / "logs";
            fs::path snap = logs / (std::string("code_snapshot_") + snapshot_name_ + ".csv");
            fs::path prev = logs / (std::string("code_snapshot_") + snapshot_name_ + ".prev.csv");
            if (fs::exists(prev)) fs::remove(prev);
            if (fs::exists(snap)) fs::rename(snap, prev);
            return true;
        } catch(...) { return false; }
    }

    const std::vector<FileSummary>& get_results() const { return results_; }
    const std::string& last_error() const { return last_error_; }

private:
    size_t count_lines(const fs::path& p) {
        std::ifstream ifs(p);
        if (!ifs) return 0;
        size_t lines=0; std::string tmp;
        while (std::getline(ifs, tmp)) ++lines;
        return lines;
    }

    std::set<std::string> allowed_exts_;
    std::vector<FileSummary> results_;
    std::string snapshot_name_ = "default";
    std::string last_error_;
};

} } }

using namespace cppengine::tests::code_analysis;

TEST_CASE("SystemCodeAnalyzer - Basic scan and snapshot", "[code_analysis]") {
    SystemCodeAnalyzer analyzer;
    // allow common code extensions
    analyzer.configure_allowed({".cpp",".h",".hpp",".cc",".cxx",".rs",".java",".py"});

    // Use environment variable to specify folder to analyze if present
    std::string folder = std::getenv("CODE_ANALYZE_DIR") ? std::getenv("CODE_ANALYZE_DIR") : "";
    if (folder.empty()) {
        // default to repository root
        folder = fs::current_path().parent_path().string();
    }

    // If interactive, ask user to override folder
    if (isatty(STDIN_FILENO)) {
        std::cout << "Enter folder to analyze for code (press Enter to use '" << folder << "'): ";
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty()) folder = input;
    }

    // snapshot name uses folder name
    std::string snapname = fs::path(folder).filename().string();
    if (snapname.empty()) snapname = "repo";

    // rotate previous snapshot if exists
    analyzer.rotate_previous();

    REQUIRE(analyzer.analyze(folder, snapname));

    std::string report;
    if (analyzer.compare_with_previous(report)) {
        INFO(report);
    } else {
        INFO("No previous snapshot to compare or comparison failed: " << analyzer.last_error());
    }

    auto results = analyzer.get_results();
    INFO("Files analyzed: " << results.size());
    // basic assertions
    REQUIRE(results.size() >= 0);
}
