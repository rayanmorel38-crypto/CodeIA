#ifndef CPP_ENGINE_OPTIMIZATION_PERFORMANCE_OPTIMIZER_H
#define CPP_ENGINE_OPTIMIZATION_PERFORMANCE_OPTIMIZER_H

#include <string>
#include <vector>
#include <cstdint>

namespace cppengine {
namespace optimization {

/**
 * PerformanceOptimizer - Resource management and caching
 * GPU acceleration, caching strategies, batch processing, memory management
 */
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    // GPU acceleration
    bool enable_gpu_acceleration();
    bool is_gpu_available() const;
    std::string get_gpu_info() const;
    
    // Caching
    bool enable_caching(size_t max_cache_size_mb);
    bool clear_cache();
    bool has_cached_result(const std::string& hash) const;
    
    // Batch processing
    void set_max_batch_size(int size);
    bool process_batch(const std::vector<std::string>& tasks);
    
    // Resource management
    void set_max_memory_mb(size_t limit);
    void set_max_threads(int count);
    size_t get_memory_usage() const;
    
    // Performance metrics
    double get_average_generation_time() const;
    double get_cache_hit_rate() const;
    
private:
    bool gpu_enabled_;
    size_t max_cache_size_mb_;
    int max_threads_;
    size_t max_memory_mb_;
};

} // namespace optimization
} // namespace cppengine

#endif // CPP_ENGINE_OPTIMIZATION_PERFORMANCE_OPTIMIZER_H
