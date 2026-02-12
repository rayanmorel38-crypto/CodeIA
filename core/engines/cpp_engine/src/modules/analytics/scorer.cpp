#include <iostream>
#include <string>
#include <cmath>

namespace cpp_engine { namespace modules { namespace analytics {

class Scorer {
public:
    static std::string name() { return "scorer"; }

    // Simple scoring heuristic: map metric string to score in [0,1]
    static double score(const std::string &metric) {
        double s = 0.0;
        for (char c : metric) s += static_cast<unsigned char>(c);
        s = std::fmod(s, 1000.0) / 1000.0;
        std::cout << "[Scorer] metric='" << metric << "' -> score=" << s << std::endl;
        return s;
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::analytics::Scorer::score("<item>");
}
