#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <numeric>
#include <cmath>

namespace cppengine { namespace modules { namespace analytics {

class Analyser {
public:
    static std::string name() { return "analyser"; }

    // Parse comma-separated numbers and report basic stats
    static void analyse(const std::string &csv_numbers) {
        std::istringstream iss(csv_numbers);
        std::string token;
        std::vector<double> values;
        while (std::getline(iss, token, ',')) {
            try { values.push_back(std::stod(token)); } catch(...) {}
        }
        if (values.empty()) {
            std::cout << "[Analyser] no numeric data provided" << std::endl;
            return;
        }
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        double var = 0.0;
        for (double v : values) var += (v - mean)*(v - mean);
        var /= values.size();
        double stddev = std::sqrt(var);
        std::cout << "[Analyser] count=" << values.size() << " mean=" << mean << " stddev=" << stddev << std::endl;
    }
};

} } }

extern "C" void register_module() {
    // registration hook for dynamic module systems (no-op for static build)
}
