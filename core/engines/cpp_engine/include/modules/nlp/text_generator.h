#ifndef CPP_ENGINE_MODULES_NLP_TEXT_GENERATOR_H
#define CPP_ENGINE_MODULES_NLP_TEXT_GENERATOR_H

#include <string>

namespace cppengine { namespace modules { namespace nlp {

class TextGenerator {
public:
    static std::string generate_response(const std::string& prompt);
    static std::string generate_text(const std::string& prompt, int max_length = 100);
};

} } }

#endif // CPP_ENGINE_MODULES_NLP_TEXT_GENERATOR_H