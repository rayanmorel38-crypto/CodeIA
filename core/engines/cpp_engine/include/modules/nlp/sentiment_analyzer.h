#ifndef CPP_ENGINE_MODULES_NLP_SENTIMENT_ANALYZER_H
#define CPP_ENGINE_MODULES_NLP_SENTIMENT_ANALYZER_H

#include <string>

namespace cppengine { namespace modules { namespace nlp {

enum class Sentiment {
    POSITIVE,
    NEGATIVE,
    NEUTRAL
};

struct SentimentResult {
    Sentiment sentiment;
    double confidence;
    std::string explanation;
};

class SentimentAnalyzer {
public:
    static SentimentResult analyze(const std::string& text);
    static Sentiment classify_sentiment(const std::string& text);
};

} } }

#endif // CPP_ENGINE_MODULES_NLP_SENTIMENT_ANALYZER_H