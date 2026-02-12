#ifndef CPP_ENGINE_MODULES_NLP_TRANSLATOR_H
#define CPP_ENGINE_MODULES_NLP_TRANSLATOR_H

#include <string>

namespace cppengine { namespace modules { namespace nlp {

class Translator {
public:
    static std::string name();
    static std::string translate(const std::string &text, const std::string &to);
};

class SentimentAnalyzer {
public:
    static std::string analyze(const std::string &text);
};

class TextGenerator {
public:
    static std::string generate_response(const std::string &input);
};

} } }

#endif // CPP_ENGINE_MODULES_NLP_TRANSLATOR_H