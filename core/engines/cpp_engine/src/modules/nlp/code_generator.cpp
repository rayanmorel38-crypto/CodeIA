#include <iostream>
#include <string>

namespace cpp_engine { namespace modules { namespace nlp {

class CodeGenerator {
public:
    static std::string name() { return "code_generator"; }
    static std::string generate(const std::string &spec) {
        std::cout << "[CodeGenerator] generate code for: " << spec << "\n";
        return "// generated code";
    }
};

} } }

extern "C" void register_module() {
    cpp_engine::modules::nlp::CodeGenerator::generate("<spec>");
}
