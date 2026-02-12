#ifndef CPP_ENGINE_MODELS_NEURAL_NETWORK_H
#define CPP_ENGINE_MODELS_NEURAL_NETWORK_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cmath>
#include <random>
#include <nlohmann/json.hpp>

namespace cppengine {
namespace models {

using json = nlohmann::json;

// Simple Dense Layer implementation
class DenseLayer {
public:
    DenseLayer(int input_size, int output_size, const std::string& activation = "relu");
    
    std::vector<float> forward(const std::vector<float>& input);
    void backward(const std::vector<float>& grad_output, float learning_rate);
    
    void save(const std::string& path) const;
    void load(const std::string& path);
    
    std::vector<float> get_weights() const;
    void set_weights(const std::vector<float>& weights);

private:
    std::vector<std::vector<float>> weights_;  // [output_size][input_size]
    std::vector<float> biases_;                // [output_size]
    std::vector<float> last_input_;
    std::vector<float> last_output_;
    std::string activation_;
    
    std::vector<float> activate(const std::vector<float>& x);
    std::vector<float> activate_derivative(const std::vector<float>& x);
};

// Neural Network implementation with configurable layers
class NeuralNetwork {
public:
    NeuralNetwork();
    
    // Architecture builder
    void add_layer(int input_size, int output_size, const std::string& activation = "relu");
    
    // Training
    void train(const std::vector<std::vector<float>>& X_train, 
               const std::vector<std::vector<float>>& y_train,
               int epochs = 10, float learning_rate = 0.01, int batch_size = 32);
    
    // Inference
    std::vector<float> predict(const std::vector<float>& input);
    
    // Evaluation
    float evaluate(const std::vector<std::vector<float>>& X_test,
                   const std::vector<std::vector<float>>& y_test);
    
    // Incremental learning (single sample)
    void learn_sample(const std::vector<float>& input, const std::vector<float>& target, 
                      float learning_rate = 0.01);
    
    // Persistence
    void save(const std::string& path) const;
    void load(const std::string& path);
    
    std::string info() const;
    
private:
    std::vector<std::unique_ptr<DenseLayer>> layers_;
    std::mt19937 rng_;
    int total_samples_trained_;
    
    float mse_loss(const std::vector<float>& predicted, const std::vector<float>& target);
    std::vector<float> mse_gradient(const std::vector<float>& predicted, const std::vector<float>& target);
};

} // namespace models
} // namespace cppengine

#endif // CPP_ENGINE_MODELS_NEURAL_NETWORK_H
