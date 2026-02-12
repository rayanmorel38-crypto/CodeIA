#include "models/neural_network.h"
#include "utils/logger.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace cppengine {
namespace models {

// ============ DenseLayer Implementation ============

DenseLayer::DenseLayer(int input_size, int output_size, const std::string& activation)
    : weights_(output_size, std::vector<float>(input_size, 0.0f)),
      biases_(output_size, 0.0f), 
      last_input_(input_size, 0.0f), 
      last_output_(output_size, 0.0f),
      activation_(activation) {
    
    // Xavier initialization
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f / std::sqrt(input_size), 
                                               1.0f / std::sqrt(input_size));
    
    // Initialize weights with Xavier initialization
    for (int i = 0; i < output_size; ++i) {
        for (int j = 0; j < input_size; ++j) {
            weights_[i][j] = dist(rng);
        }
    }
}

std::vector<float> DenseLayer::activate(const std::vector<float>& x) {
    std::vector<float> result = x;
    if (activation_ == "relu") {
        for (auto& val : result) {
            val = std::max(0.0f, val);
        }
    } else if (activation_ == "sigmoid") {
        for (auto& val : result) {
            val = 1.0f / (1.0f + std::exp(-val));
        }
    } else if (activation_ == "tanh") {
        for (auto& val : result) {
            val = std::tanh(val);
        }
    }
    return result;
}

std::vector<float> DenseLayer::activate_derivative(const std::vector<float>& x) {
    std::vector<float> result(x.size());
    if (activation_ == "relu") {
        for (size_t i = 0; i < x.size(); ++i) {
            result[i] = x[i] > 0.0f ? 1.0f : 0.0f;
        }
    } else if (activation_ == "sigmoid") {
        for (size_t i = 0; i < x.size(); ++i) {
            float sig = 1.0f / (1.0f + std::exp(-x[i]));
            result[i] = sig * (1.0f - sig);
        }
    } else if (activation_ == "tanh") {
        for (size_t i = 0; i < x.size(); ++i) {
            float t = std::tanh(x[i]);
            result[i] = 1.0f - t * t;
        }
    }
    return result;
}

std::vector<float> DenseLayer::forward(const std::vector<float>& input) {
    last_input_ = input;
    
    int output_size = weights_.size();
    std::vector<float> z(output_size, 0.0f);
    
    // Matrix multiplication: z = W * x + b
    for (int i = 0; i < output_size; ++i) {
        z[i] = biases_[i];
        for (size_t j = 0; j < input.size(); ++j) {
            z[i] += weights_[i][j] * input[j];
        }
    }
    
    last_output_ = activate(z);
    return last_output_;
}

void DenseLayer::backward(const std::vector<float>& grad_output, float learning_rate) {
    int input_size = last_input_.size();
    int output_size = weights_.size();
    
    // Update biases
    for (int i = 0; i < output_size; ++i) {
        biases_[i] -= learning_rate * grad_output[i];
    }
    
    // Update weights
    for (int i = 0; i < output_size; ++i) {
        for (int j = 0; j < input_size; ++j) {
            weights_[i][j] -= learning_rate * grad_output[i] * last_input_[j];
        }
    }
}

void DenseLayer::save(const std::string& path) const {
    json j;
    j["activation"] = activation_;
    j["weights"] = weights_;
    j["biases"] = biases_;
    
    std::ofstream file(path);
    file << j.dump(4);
    file.close();
}

void DenseLayer::load(const std::string& path) {
    std::ifstream file(path);
    json j;
    file >> j;
    file.close();
    
    activation_ = j["activation"];
    weights_ = j["weights"].get<decltype(weights_)>();
    biases_ = j["biases"].get<decltype(biases_)>();
}

std::vector<float> DenseLayer::get_weights() const {
    std::vector<float> result;
    for (const auto& row : weights_) {
        result.insert(result.end(), row.begin(), row.end());
    }
    result.insert(result.end(), biases_.begin(), biases_.end());
    return result;
}

void DenseLayer::set_weights(const std::vector<float>& weights) {
    size_t idx = 0;
    for (auto& row : weights_) {
        for (auto& val : row) {
            if (idx < weights.size()) val = weights[idx++];
        }
    }
    for (auto& val : biases_) {
        if (idx < weights.size()) val = weights[idx++];
    }
}

// ============ NeuralNetwork Implementation ============

NeuralNetwork::NeuralNetwork() : rng_(std::random_device{}()), total_samples_trained_(0) {}

void NeuralNetwork::add_layer(int input_size, int output_size, const std::string& activation) {
    layers_.push_back(std::make_unique<DenseLayer>(input_size, output_size, activation));
}

std::vector<float> NeuralNetwork::predict(const std::vector<float>& input) {
    std::vector<float> x = input;
    for (auto& layer : layers_) {
        x = layer->forward(x);
    }
    return x;
}

float NeuralNetwork::mse_loss(const std::vector<float>& predicted, const std::vector<float>& target) {
    float loss = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - target[i];
        loss += diff * diff;
    }
    return loss / predicted.size();
}

std::vector<float> NeuralNetwork::mse_gradient(const std::vector<float>& predicted, 
                                               const std::vector<float>& target) {
    std::vector<float> grad(predicted.size());
    for (size_t i = 0; i < predicted.size(); ++i) {
        grad[i] = 2.0f * (predicted[i] - target[i]) / predicted.size();
    }
    return grad;
}

void NeuralNetwork::train(const std::vector<std::vector<float>>& X_train,
                          const std::vector<std::vector<float>>& y_train,
                          int epochs, float learning_rate, int batch_size) {
    if (layers_.empty()) {
        cpp_engine::utils::Logger::instance().error("NeuralNetwork: no layers configured");
        return;
    }
    
    for (int epoch = 0; epoch < epochs; ++epoch) {
        float epoch_loss = 0.0f;
        int num_batches = (X_train.size() + batch_size - 1) / batch_size;
        
        for (int batch = 0; batch < num_batches; ++batch) {
            int start_idx = batch * batch_size;
            int end_idx = std::min(start_idx + batch_size, (int)X_train.size());
            
            std::vector<float> batch_loss_sum(y_train[0].size(), 0.0f);
            
            for (int i = start_idx; i < end_idx; ++i) {
                auto predicted = predict(X_train[i]);
                auto loss_grad = mse_gradient(predicted, y_train[i]);
                epoch_loss += mse_loss(predicted, y_train[i]);
                
                // Backward pass
                std::vector<float> grad = loss_grad;
                for (int l = layers_.size() - 1; l >= 0; --l) {
                    layers_[l]->backward(grad, learning_rate);
                }
            }
        }
        
        total_samples_trained_ += X_train.size();
        if (epoch % std::max(1, epochs / 10) == 0) {
            cpp_engine::utils::Logger::instance().info("Epoch " + std::to_string(epoch) + "/" + std::to_string(epochs) +
                                         " - Loss: " + std::to_string(epoch_loss / num_batches));
        }
    }
}

void NeuralNetwork::learn_sample(const std::vector<float>& input, const std::vector<float>& target,
                                 float learning_rate) {
    auto predicted = predict(input);
    auto loss_grad = mse_gradient(predicted, target);
    
    // Backward pass
    std::vector<float> grad = loss_grad;
    for (int l = layers_.size() - 1; l >= 0; --l) {
        layers_[l]->backward(grad, learning_rate);
    }
    
    total_samples_trained_++;
}

float NeuralNetwork::evaluate(const std::vector<std::vector<float>>& X_test,
                             const std::vector<std::vector<float>>& y_test) {
    float total_loss = 0.0f;
    for (size_t i = 0; i < X_test.size(); ++i) {
        auto predicted = predict(X_test[i]);
        total_loss += mse_loss(predicted, y_test[i]);
    }
    return total_loss / X_test.size();
}

void NeuralNetwork::save(const std::string& path) const {
    json j;
    j["layers_count"] = layers_.size();
    j["total_samples_trained"] = total_samples_trained_;
    
    for (size_t i = 0; i < layers_.size(); ++i) {
        j["layer_" + std::to_string(i)] = layers_[i]->get_weights();
    }
    
    std::ofstream file(path);
    file << j.dump(4);
    file.close();
    
    cpp_engine::utils::Logger::instance().info("NeuralNetwork saved to: " + path);
}

void NeuralNetwork::load(const std::string& path) {
    std::ifstream file(path);
    json j;
    file >> j;
    file.close();
    
    total_samples_trained_ = j["total_samples_trained"];
    
    for (size_t i = 0; i < layers_.size(); ++i) {
        auto weights = j["layer_" + std::to_string(i)].get<std::vector<float>>();
        layers_[i]->set_weights(weights);
    }
    
    cpp_engine::utils::Logger::instance().info("NeuralNetwork loaded from: " + path);
}

std::string NeuralNetwork::info() const {
    json j;
    j["layers"] = layers_.size();
    j["total_samples_trained"] = total_samples_trained_;
    j["status"] = "ready";
    return j.dump(4);
}

} // namespace models
} // namespace cppengine
