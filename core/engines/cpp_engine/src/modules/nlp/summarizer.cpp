#include <iostream>
#include <string>

namespace cpp_engine { namespace modules { namespace nlp {

class Summarizer {
public:
    static std::string name() { return "summarizer"; }
    static std::string summarize(const std::string &text) {
        std::cout << "[Summarizer] summarize: " << text << "\n";
        return "summary";
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::nlp::Summarizer::summarize("<text>");
}
