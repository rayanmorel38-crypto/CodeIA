#include "network/http_server.h"
#include "network/validation_endpoint.h"

#include <nlohmann/json.hpp>
#include <httplib.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
using Clock = std::chrono::system_clock;
using SteadyClock = std::chrono::steady_clock;

long long now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now().time_since_epoch()).count();
}

json envelope_ok(const json& data = json::object()) {
    return json{
        {"success", true},
        {"engine", "cpp_engine"},
        {"timestamp_ms", now_ms()},
        {"data", data}
    };
}

json envelope_error(const std::string& message, int code = 500, const json& details = json::object()) {
    return json{
        {"success", false},
        {"engine", "cpp_engine"},
        {"timestamp_ms", now_ms()},
        {"error", {
            {"code", code},
            {"message", message},
            {"details", details}
        }}
    };
}

std::string get_env_or(const char* key, const std::string& fallback) {
    const char* v = std::getenv(key);
    if (!v || std::string(v).empty()) return fallback;
    return std::string(v);
}

bool is_orchestrator_mode() {
    return get_env_or("CODEIA_LAUNCH_MODE", "") == "orchestrator";
}

bool authorize_orchestrator(const httplib::Request& req, httplib::Response& res) {
    const std::string expected = get_env_or("CODEIA_ORCHESTRATOR_TOKEN", "");
    if (expected.empty()) {
        res.status = 503;
        res.set_content(envelope_error("server misconfigured: missing CODEIA_ORCHESTRATOR_TOKEN", 503).dump(), "application/json");
        return false;
    }

    std::string provided;
    if (req.has_header("X-Orchestrator-Token")) {
        provided = req.get_header_value("X-Orchestrator-Token");
    } else if (req.has_header("Authorization")) {
        const std::string auth = req.get_header_value("Authorization");
        const std::string prefix = "Bearer ";
        if (auth.rfind(prefix, 0) == 0) {
            provided = auth.substr(prefix.size());
        }
    }

    if (provided != expected) {
        res.status = 401;
        res.set_content(envelope_error("unauthorized", 401).dump(), "application/json");
        return false;
    }
    return true;
}

int get_env_int_or(const char* key, int fallback) {
    const char* v = std::getenv(key);
    if (!v || std::string(v).empty()) return fallback;
    try { return std::stoi(v); } catch (...) { return fallback; }
}

std::string read_all_from_fd(int fd) {
    std::string out;
    char buffer[4096];
    while (true) {
        const ssize_t n = ::read(fd, buffer, sizeof(buffer));
        if (n <= 0) break;
        out.append(buffer, static_cast<size_t>(n));
    }
    return out;
}

struct TaskMetrics {
    long long start_time_ms = 0;
    long long end_time_ms = 0;
    int peak_memory_kb = 0;
    int cpu_percent = 0;
    double io_throughput_mb_s = 0.0;

    json to_json() const {
        json j;
        j["start_time_ms"] = start_time_ms;
        j["end_time_ms"] = end_time_ms;
        j["duration_ms"] = (end_time_ms > start_time_ms) ? (end_time_ms - start_time_ms) : 0;
        j["peak_memory_kb"] = peak_memory_kb;
        j["cpu_percent"] = cpu_percent;
        j["io_throughput_mb_s"] = io_throughput_mb_s;
        return j;
    }
};

struct TaskState {
    std::string task_id;
    std::string status = "queued";
    pid_t pid = -1;
    std::vector<std::string> command;
    std::string stdout_text;
    std::string stderr_text;
    int exit_code = -1;
    long long created_at_ms = now_ms();
    int timeout_seconds = 60;
    TaskMetrics metrics;
    json timeline = json::array();

    json to_json(bool include_output = true) const {
        json j;
        j["task_id"] = task_id;
        j["status"] = status;
        j["pid"] = pid;
        j["command"] = command;
        j["exit_code"] = exit_code;
        j["created_at_ms"] = created_at_ms;
        j["elapsed_seconds"] = std::max(0.0, (now_ms() - static_cast<double>(created_at_ms)) / 1000.0);
        j["timeout_seconds"] = timeout_seconds;
        j["metrics"] = metrics.to_json();
        if (include_output) {
            j["stdout"] = stdout_text;
            j["stderr"] = stderr_text;
            j["timeline"] = timeline;
        }
        return j;
    }
};

class TaskLogger {
public:
    static void log_event(TaskState& task, const std::string& event, const json& data = json::object()) {
        task.timeline.push_back(json{{"ts_ms", now_ms()}, {"event", event}, {"data", data}});
        flush(task);
    }

private:
    static void flush(const TaskState& task) {
        std::error_code ec;
        fs::create_directories(fs::current_path() / "logs", ec);
        std::ofstream out(fs::current_path() / "logs" / (task.task_id + ".json"), std::ios::trunc);
        if (out.is_open()) out << task.to_json(true).dump(2);
    }
};

struct TaskStore {
    std::map<std::string, TaskState> tasks;
    std::mutex mtx;
} g_store;

std::string make_task_id() {
    static std::atomic<unsigned long long> seq{1};
    return "task-" + std::to_string(now_ms()) + "-" + std::to_string(seq.fetch_add(1));
}

void cleanup_old_tasks(int retention_seconds) {
    const long long cutoff = now_ms() - static_cast<long long>(retention_seconds) * 1000LL;
    std::lock_guard<std::mutex> lock(g_store.mtx);
    for (auto it = g_store.tasks.begin(); it != g_store.tasks.end();) {
        const bool old = it->second.created_at_ms < cutoff;
        const bool done = it->second.status != "running" && it->second.status != "queued";
        if (old && done) it = g_store.tasks.erase(it);
        else ++it;
    }
}

json aggregate_metrics_locked() {
    json out;
    size_t completed = 0, failed = 0, timeout = 0;
    long long total_duration = 0;
    long long peak_mem = 0;
    for (const auto& kv : g_store.tasks) {
        const auto& t = kv.second;
        if (t.status == "completed") completed++;
        if (t.status == "failed") failed++;
        if (t.status == "timeout") timeout++;
        if (t.metrics.end_time_ms > t.metrics.start_time_ms) {
            total_duration += (t.metrics.end_time_ms - t.metrics.start_time_ms);
        }
        peak_mem = std::max<long long>(peak_mem, t.metrics.peak_memory_kb);
    }
    out["total_tasks"] = g_store.tasks.size();
    out["completed"] = completed;
    out["failed"] = failed;
    out["timeout"] = timeout;
    out["avg_duration_ms"] = completed ? (total_duration / static_cast<long long>(completed)) : 0;
    out["peak_memory_kb"] = peak_mem;
    return out;
}

std::pair<int, struct rusage> wait_with_timeout(pid_t pid, int timeout_seconds) {
    struct rusage ru{};
    auto deadline = SteadyClock::now() + std::chrono::seconds(timeout_seconds);
    int status = 0;
    while (SteadyClock::now() < deadline) {
        const pid_t r = ::wait4(pid, &status, WNOHANG, &ru);
        if (r == pid) return {status, ru};
        if (r < 0) return {-1, ru};
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ::kill(pid, SIGKILL);
    ::wait4(pid, &status, 0, &ru);
    return {INT32_MIN, ru};
}
}

namespace cppengine {
namespace network {

HttpServer::HttpServer() {
    config_.host = get_env_or("CPP_ENGINE_HOST", "127.0.0.1");
    config_.port = get_env_int_or("CPP_ENGINE_PORT", 3004);
    config_.num_threads = get_env_int_or("CPP_ENGINE_THREADS", 4);
    config_.cpp_bin = get_env_or("CPP_ENGINE_BIN", "./build/bin/image_video_generator");
    config_.default_timeout_seconds = get_env_int_or("TASK_TIMEOUT", 60);
    config_.retention_seconds = get_env_int_or("TASK_RETENTION_SECONDS", 3600);
}

HttpServer::HttpServer(const Config& config) : config_(config) {
    if (config_.cpp_bin.empty()) config_.cpp_bin = get_env_or("CPP_ENGINE_BIN", "./build/bin/image_video_generator");
    if (config_.default_timeout_seconds <= 0) config_.default_timeout_seconds = 60;
    if (config_.retention_seconds <= 0) config_.retention_seconds = 3600;
}

HttpServer::~HttpServer() { stop(); }

void HttpServer::start() {
    if (!is_orchestrator_mode()) {
        std::cerr << "Refusing to start: CODEIA_LAUNCH_MODE must be 'orchestrator'" << std::endl;
        running_.store(false);
        return;
    }

    auto server = std::make_shared<httplib::Server>();
    ValidationEndpoint validator;
    running_.store(true);
    cleanup_running_.store(true);

    cleanup_thread_ = std::thread([this]() {
        while (cleanup_running_.load()) {
            cleanup_old_tasks(config_.retention_seconds);
            std::this_thread::sleep_for(std::chrono::minutes(5));
        }
    });

    server->Get("/health", [this](const httplib::Request&, httplib::Response& res) {
        const json data = {
            {"service", "cpp_engine"},
            {"ready", true},
            {"native_http_server", true},
            {"python_wrapper_enabled", false},
            {"launch_mode", get_env_or("CODEIA_LAUNCH_MODE", "")},
            {"port", config_.port},
            {"cpp_bin", config_.cpp_bin}
        };
        res.set_content(envelope_ok(data).dump(), "application/json");
    });

    server->Post("/process", [this](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        json payload;
        try {
            payload = json::parse(req.body);
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(envelope_error(std::string("Invalid JSON: ") + e.what(), 400).dump(), "application/json");
            return;
        }

        int timeout = payload.value("timeout", config_.default_timeout_seconds);
        if (timeout <= 0) timeout = config_.default_timeout_seconds;

        std::vector<std::string> args;
        if (payload.contains("command") && payload["command"].is_array()) {
            for (const auto& it : payload["command"]) args.push_back(it.get<std::string>());
        } else {
            const std::string filter = payload.value("filter", "");
            const std::string input = payload.value("input", "");
            const std::string output = payload.value("output", "");
            if (!filter.empty()) { args.push_back("--filter"); args.push_back(filter); }
            if (!input.empty()) { args.push_back("--input"); args.push_back(input); }
            if (!output.empty()) { args.push_back("--output"); args.push_back(output); }
            if (payload.contains("args") && payload["args"].is_array()) {
                for (const auto& it : payload["args"]) args.push_back(it.get<std::string>());
            }
        }

        if (args.empty()) {
            res.status = 400;
            res.set_content(envelope_error("Missing command or advanced params (filter/input/output)", 400).dump(), "application/json");
            return;
        }
        if (!fs::exists(config_.cpp_bin)) {
            res.status = 500;
            res.set_content(envelope_error("CPP binary not found", 500, json{{"cpp_bin", config_.cpp_bin}}).dump(), "application/json");
            return;
        }

        TaskState task;
        task.task_id = make_task_id();
        task.timeout_seconds = timeout;
        task.command.push_back(config_.cpp_bin);
        task.command.insert(task.command.end(), args.begin(), args.end());
        TaskLogger::log_event(task, "task_submitted", json{{"timeout", timeout}, {"argc", task.command.size()}});

        {
            std::lock_guard<std::mutex> lock(g_store.mtx);
            g_store.tasks[task.task_id] = task;
        }

        const std::string task_id = task.task_id;
        std::thread([task_id]() {
            std::vector<std::string> command;
            int timeout_seconds = 60;
            {
                std::lock_guard<std::mutex> lock(g_store.mtx);
                auto it = g_store.tasks.find(task_id);
                if (it == g_store.tasks.end()) return;
                it->second.status = "running";
                it->second.metrics.start_time_ms = now_ms();
                TaskLogger::log_event(it->second, "task_started");
                command = it->second.command;
                timeout_seconds = it->second.timeout_seconds;
            }

            int out_pipe[2] = {-1, -1};
            int err_pipe[2] = {-1, -1};
            if (::pipe(out_pipe) != 0 || ::pipe(err_pipe) != 0) {
                std::lock_guard<std::mutex> lock(g_store.mtx);
                auto it = g_store.tasks.find(task_id);
                if (it != g_store.tasks.end()) {
                    it->second.status = "failed";
                    it->second.stderr_text = "pipe() failed";
                    it->second.metrics.end_time_ms = now_ms();
                    TaskLogger::log_event(it->second, "task_failed", json{{"reason", "pipe_failed"}});
                }
                return;
            }

            const pid_t pid = ::fork();
            if (pid < 0) {
                ::close(out_pipe[0]); ::close(out_pipe[1]);
                ::close(err_pipe[0]); ::close(err_pipe[1]);
                std::lock_guard<std::mutex> lock(g_store.mtx);
                auto it = g_store.tasks.find(task_id);
                if (it != g_store.tasks.end()) {
                    it->second.status = "failed";
                    it->second.stderr_text = "fork() failed";
                    it->second.metrics.end_time_ms = now_ms();
                    TaskLogger::log_event(it->second, "task_failed", json{{"reason", "fork_failed"}});
                }
                return;
            }

            if (pid == 0) {
                ::dup2(out_pipe[1], STDOUT_FILENO);
                ::dup2(err_pipe[1], STDERR_FILENO);
                ::close(out_pipe[0]); ::close(out_pipe[1]);
                ::close(err_pipe[0]); ::close(err_pipe[1]);
                std::vector<char*> argv;
                argv.reserve(command.size() + 1);
                for (auto& s : command) argv.push_back(const_cast<char*>(s.c_str()));
                argv.push_back(nullptr);
                ::execvp(argv[0], argv.data());
                std::cerr << "execvp failed: " << std::strerror(errno) << std::endl;
                _exit(127);
            }

            ::close(out_pipe[1]);
            ::close(err_pipe[1]);
            {
                std::lock_guard<std::mutex> lock(g_store.mtx);
                auto it = g_store.tasks.find(task_id);
                if (it != g_store.tasks.end()) {
                    it->second.pid = pid;
                    TaskLogger::log_event(it->second, "process_spawned", json{{"pid", pid}});
                }
            }

            auto wait_pair = wait_with_timeout(pid, timeout_seconds);
            const int wait_status = wait_pair.first;
            const struct rusage ru = wait_pair.second;
            const std::string out = read_all_from_fd(out_pipe[0]);
            const std::string err = read_all_from_fd(err_pipe[0]);
            ::close(out_pipe[0]);
            ::close(err_pipe[0]);

            std::lock_guard<std::mutex> lock(g_store.mtx);
            auto it = g_store.tasks.find(task_id);
            if (it == g_store.tasks.end()) return;

            it->second.stdout_text = out;
            it->second.stderr_text = err;
            it->second.metrics.end_time_ms = now_ms();
            it->second.metrics.peak_memory_kb = static_cast<int>(ru.ru_maxrss);

            const double dur_s = std::max(0.001, (it->second.metrics.end_time_ms - it->second.metrics.start_time_ms) / 1000.0);
            const double io_mb = static_cast<double>(out.size() + err.size()) / (1024.0 * 1024.0);
            it->second.metrics.io_throughput_mb_s = io_mb / dur_s;
            const long user_ms = ru.ru_utime.tv_sec * 1000L + ru.ru_utime.tv_usec / 1000L;
            const long sys_ms = ru.ru_stime.tv_sec * 1000L + ru.ru_stime.tv_usec / 1000L;
            it->second.metrics.cpu_percent = static_cast<int>(std::min(100.0, ((user_ms + sys_ms) / (dur_s * 10.0))));

            if (wait_status == INT32_MIN) {
                it->second.status = "timeout";
                it->second.exit_code = -1;
                TaskLogger::log_event(it->second, "task_timeout", json{{"timeout_seconds", timeout_seconds}});
                return;
            }
            if (wait_status == -1) {
                it->second.status = "failed";
                it->second.exit_code = -1;
                if (it->second.stderr_text.empty()) it->second.stderr_text = "wait4() failed";
                TaskLogger::log_event(it->second, "task_failed", json{{"reason", "wait_failed"}});
                return;
            }
            if (WIFEXITED(wait_status)) {
                it->second.exit_code = WEXITSTATUS(wait_status);
                it->second.status = (it->second.exit_code == 0) ? "completed" : "failed";
                TaskLogger::log_event(it->second, (it->second.status == "completed") ? "task_completed" : "task_failed", json{{"exit_code", it->second.exit_code}});
            } else if (WIFSIGNALED(wait_status)) {
                it->second.exit_code = 128 + WTERMSIG(wait_status);
                it->second.status = "failed";
                TaskLogger::log_event(it->second, "task_failed", json{{"signal", WTERMSIG(wait_status)}});
            } else {
                it->second.status = "failed";
                it->second.exit_code = -1;
                TaskLogger::log_event(it->second, "task_failed", json{{"reason", "unknown_wait_status"}});
            }
        }).detach();

        res.set_content(
            envelope_ok(json{{"task_id", task_id}, {"status", "accepted"}, {"status_url", "/status/" + task_id}, {"results_url", "/results/" + task_id}, {"metrics_url", "/metrics/" + task_id}, {"timeout_seconds", timeout}}).dump(),
            "application/json"
        );
    });

    server->Get(R"(/status/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        const std::string task_id = req.matches.size() > 1 ? req.matches[1].str() : "";
        std::lock_guard<std::mutex> lock(g_store.mtx);
        auto it = g_store.tasks.find(task_id);
        if (it == g_store.tasks.end()) {
            res.status = 404;
            res.set_content(envelope_error("task not found", 404, json{{"task_id", task_id}}).dump(), "application/json");
            return;
        }
        res.set_content(envelope_ok(it->second.to_json(true)).dump(), "application/json");
    });

    server->Get(R"(/results/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        const std::string task_id = req.matches.size() > 1 ? req.matches[1].str() : "";
        std::lock_guard<std::mutex> lock(g_store.mtx);
        auto it = g_store.tasks.find(task_id);
        if (it == g_store.tasks.end()) {
            res.status = 404;
            res.set_content(envelope_error("task not found", 404, json{{"task_id", task_id}}).dump(), "application/json");
            return;
        }

        const json data = {
            {"task_id", it->second.task_id},
            {"status", it->second.status},
            {"exit_code", it->second.exit_code},
            {"stdout", it->second.stdout_text},
            {"stderr", it->second.stderr_text}
        };
        res.set_content(envelope_ok(data).dump(), "application/json");
    });

    server->Get("/tasks", [](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        int limit = 50;
        if (req.has_param("limit")) {
            try { limit = std::max(1, std::stoi(req.get_param_value("limit"))); } catch (...) { limit = 50; }
        }
        json out;
        out["tasks"] = json::array();
        std::lock_guard<std::mutex> lock(g_store.mtx);
        int count = 0;
        for (auto it = g_store.tasks.rbegin(); it != g_store.tasks.rend() && count < limit; ++it, ++count) {
            out["tasks"].push_back(it->second.to_json(false));
        }
        out["total"] = g_store.tasks.size();
        res.set_content(envelope_ok(out).dump(), "application/json");
    });

    server->Get("/metrics", [](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        std::lock_guard<std::mutex> lock(g_store.mtx);
        res.set_content(envelope_ok(aggregate_metrics_locked()).dump(), "application/json");
    });

    server->Get(R"(/metrics/(.+))", [](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        const std::string task_id = req.matches.size() > 1 ? req.matches[1].str() : "";
        std::lock_guard<std::mutex> lock(g_store.mtx);
        auto it = g_store.tasks.find(task_id);
        if (it == g_store.tasks.end()) {
            res.status = 404;
            res.set_content(envelope_error("task not found", 404, json{{"task_id", task_id}}).dump(), "application/json");
            return;
        }
        res.set_content(envelope_ok(it->second.metrics.to_json()).dump(), "application/json");
    });

    server->Post("/validate", [&validator](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        try {
            auto payload = json::parse(req.body);
            res.set_content(envelope_ok(validator.handle_validate_request(payload)).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(envelope_error(e.what(), 400).dump(), "application/json");
        }
    });

    server->Post("/validate/batch", [&validator](const httplib::Request& req, httplib::Response& res) {
        if (!authorize_orchestrator(req, res)) {
            return;
        }

        try {
            auto payload = json::parse(req.body);
            res.set_content(envelope_ok(validator.handle_batch_validate_request(payload)).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(envelope_error(e.what(), 400).dump(), "application/json");
        }
    });

    std::cout << "cpp_engine native HTTP server on http://" << config_.host << ":" << config_.port << std::endl;
    std::cout << "binary=" << config_.cpp_bin << std::endl;

    server->listen(config_.host.c_str(), config_.port);
    running_.store(false);
    cleanup_running_.store(false);
    if (cleanup_thread_.joinable()) cleanup_thread_.join();
}

void HttpServer::stop() {
    cleanup_running_.store(false);
    running_.store(false);
    if (cleanup_thread_.joinable()) cleanup_thread_.join();
}

bool HttpServer::is_running() const { return running_.load(); }

void HttpServer::on(const std::string&, const std::string&, std::function<void(const std::string&, std::string&)>) {}

} // namespace network
} // namespace cppengine
