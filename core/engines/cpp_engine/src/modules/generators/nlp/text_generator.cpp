#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <sstream>

namespace cpp_engine { namespace modules { namespace generators {

class TextGenerator {
public:
    static std::string name() { return "text_generator"; }

    // Very small template-based generator with randomness and temperature
    static std::string generate(const std::string &prompt, double temperature = 0.7) {
        std::vector<std::string> continuations = {
            "is an interesting idea.",
            "could be explored further.",
            "has practical applications.",
            "may require more data.",
            "looks promising for prototyping."
        };

        std::mt19937 gen(static_cast<unsigned int>(std::hash<std::string>{}(prompt)));
        std::uniform_real_distribution<> ud(0.0, 1.0);

        std::ostringstream out;
        out << prompt << " ";

        int n = 1 + static_cast<int>(temperature * 2);
        for (int i = 0; i < n; ++i) {
            double r = ud(gen);
            size_t idx = static_cast<size_t>(r * continuations.size()) % continuations.size();
            out << continuations[idx] << " ";
        }

        std::string res = out.str();
        std::cout << "[TextGenerator] prompt='" << prompt << "' -> '" << res << "'" << std::endl;
        return res;
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::generators::TextGenerator::generate("Test prompt for text generator");
}
