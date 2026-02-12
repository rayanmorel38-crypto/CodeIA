#include "modules/nlp/sentiment_analyzer.h"
#include <string>
#include <algorithm>
#include <random>
#include "utils/logger.h"

namespace cppengine { namespace modules { namespace nlp {

SentimentResult SentimentAnalyzer::analyze(const std::string& text) {
    SentimentResult result;
    result.sentiment = classify_sentiment(text);
    
    // Simple mock confidence based on text length and keywords
    double base_confidence = 0.5 + (text.length() % 50) / 100.0;
    
    std::string lower_text = text;
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
    
    if (lower_text.find("good") != std::string::npos || lower_text.find("great") != std::string::npos) {
        base_confidence += 0.2;
    }
    if (lower_text.find("bad") != std::string::npos || lower_text.find("terrible") != std::string::npos) {
        base_confidence -= 0.2;
    }
    
    result.confidence = std::max(0.1, std::min(0.95, base_confidence));
    
    switch (result.sentiment) {
        case Sentiment::POSITIVE:
            result.explanation = "Text contains positive language and expressions";
            break;
        case Sentiment::NEGATIVE:
            result.explanation = "Text contains negative language and expressions";
            break;
        case Sentiment::NEUTRAL:
            result.explanation = "Text appears neutral or objective";
            break;
    }
    
    cpp_engine::utils::Logger::instance().info("[SentimentAnalyzer] analyzed text: " + text.substr(0, 30) + "...");
    return result;
}

Sentiment SentimentAnalyzer::classify_sentiment(const std::string& text) {
    std::string lower_text = text;
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
    
    int positive_words = 0;
    int negative_words = 0;
    
    // Simple keyword-based classification
    std::vector<std::string> positive_keywords = {"good", "great", "excellent", "amazing", "wonderful", "fantastic", "love", "like", "happy", "joy"};
    std::vector<std::string> negative_keywords = {"bad", "terrible", "awful", "hate", "dislike", "sad", "angry", "horrible", "worst", "suck"};
    
    for (const auto& word : positive_keywords) {
        if (lower_text.find(word) != std::string::npos) {
            positive_words++;
        }
    }
    
    for (const auto& word : negative_keywords) {
        if (lower_text.find(word) != std::string::npos) {
            negative_words++;
        }
    }
    
    if (positive_words > negative_words) {
        return Sentiment::POSITIVE;
    } else if (negative_words > positive_words) {
        return Sentiment::NEGATIVE;
    } else {
        return Sentiment::NEUTRAL;
    }
}

} } }