/// Implémentation du Deep Learning
use serde::{Serialize, Deserialize};
use rand::Rng;

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Neuron {
    pub weights: Vec<f64>,
    pub bias: f64,
    pub activation_fn: ActivationFunction,
}

#[derive(Clone, Debug, Serialize, Deserialize, PartialEq)]
pub enum ActivationFunction {
    ReLU,
    Sigmoid,
    Tanh,
    Linear,
}

#[derive(Clone, Debug, Serialize, Deserialize)]
pub struct Layer {
    pub neurons: Vec<Neuron>,
    pub input_size: usize,
    pub output_size: usize,
}

pub struct NeuralNetwork {
    pub layers: Vec<Layer>,
    pub learning_rate: f64,
    pub activations_cache: Vec<Vec<f64>>, // Cache des activations pour la backprop
}

impl Neuron {
    pub fn new(input_size: usize, activation_fn: ActivationFunction) -> Self {
        let mut rng = rand::thread_rng();
        let weights = (0..input_size)
            .map(|_| rng.gen_range(-1.0..1.0) * 0.1)
            .collect();
        
        Neuron {
            weights,
            bias: rng.gen_range(-1.0..1.0) * 0.1,
            activation_fn,
        }
    }

    fn apply_activation(&self, z: f64) -> f64 {
        match self.activation_fn {
            ActivationFunction::ReLU => z.max(0.0),
            ActivationFunction::Sigmoid => 1.0 / (1.0 + (-z).exp()),
            ActivationFunction::Tanh => z.tanh(),
            ActivationFunction::Linear => z,
        }
    }

    pub fn forward(&self, inputs: &[f64]) -> f64 {
        let z: f64 = self.weights
            .iter()
            .zip(inputs.iter())
            .map(|(w, x)| w * x)
            .sum::<f64>() + self.bias;
        self.apply_activation(z)
    }

    pub fn backward(&mut self, error: f64, inputs: &[f64], learning_rate: f64) {
        for (weight, input) in self.weights.iter_mut().zip(inputs.iter()) {
            *weight += learning_rate * error * input;
        }
        self.bias += learning_rate * error;
    }
}

impl Layer {
    pub fn new(input_size: usize, output_size: usize, activation_fn: ActivationFunction) -> Self {
        let neurons = (0..output_size)
            .map(|_| Neuron::new(input_size, activation_fn.clone()))
            .collect();
        
        Layer {
            neurons,
            input_size,
            output_size,
        }
    }

    pub fn forward(&self, inputs: &[f64]) -> Vec<f64> {
        self.neurons.iter().map(|n| n.forward(inputs)).collect()
    }

    pub fn backward(&mut self, errors: &[f64], inputs: &[f64], learning_rate: f64) {
        for (neuron, error) in self.neurons.iter_mut().zip(errors.iter()) {
            neuron.backward(*error, inputs, learning_rate);
        }
    }

    /// Calcule les gradients pour la couche précédente
    pub fn compute_input_gradients(&self, output_errors: &[f64]) -> Vec<f64> {
        let mut input_gradients = vec![0.0; self.input_size];
        
        for (neuron, error) in self.neurons.iter().zip(output_errors.iter()) {
            for (weight, grad) in neuron.weights.iter().zip(input_gradients.iter_mut()) {
                *grad += weight * error;
            }
        }
        
        input_gradients
    }
}

impl NeuralNetwork {
    pub fn new(learning_rate: f64) -> Self {
        NeuralNetwork {
            layers: Vec::new(),
            learning_rate,
            activations_cache: Vec::new(),
        }
    }

    pub fn add_layer(&mut self, input_size: usize, output_size: usize, activation_fn: ActivationFunction) {
        self.layers.push(Layer::new(input_size, output_size, activation_fn));
        self.activations_cache.push(Vec::new());
    }

    pub fn forward(&mut self, inputs: &[f64]) -> Vec<f64> {
        let mut current = inputs.to_vec();
        
        // Effacer le cache précédent et stocker les entrées
        self.activations_cache.clear();
        self.activations_cache.push(current.clone());
        
        for layer in &self.layers {
            current = layer.forward(&current);
            self.activations_cache.push(current.clone());
        }
        
        current
    }

    pub fn backward(&mut self, target: &[f64], output: &[f64]) {
        // Erreur de la couche de sortie
        let mut errors: Vec<f64> = output
            .iter()
            .zip(target.iter())
            .map(|(o, t)| t - o)
            .collect();

        // Backpropagation couche par couche, en sens inverse
        for i in (0..self.layers.len()).rev() {
            let layer = &mut self.layers[i];
            
            // Récupérer les entrées de cette couche depuis le cache
            let layer_inputs = self.activations_cache.get(i).cloned().unwrap_or_default();

            // Calculer les gradients pour l'entrée (qui devient l'erreur de la couche précédente)
            let input_errors = layer.compute_input_gradients(&errors);
            
            // Mettre à jour les poids et biais de cette couche
            layer.backward(&errors, &layer_inputs, self.learning_rate);

            // Propager l'erreur vers la couche précédente
            errors = input_errors;
        }
    }

    pub fn train(&mut self, training_data: &[(Vec<f64>, Vec<f64>)], epochs: usize) {
        for _epoch in 0..epochs {
            let mut total_loss = 0.0;
            
            for (input, target) in training_data {
                let output = self.forward(input);
                
                let loss: f64 = output
                    .iter()
                    .zip(target.iter())
                    .map(|(o, t)| (o - t).powi(2))
                    .sum::<f64>() / target.len() as f64;
                total_loss += loss;
                
                self.backward(target, &output);
            }
            
            let avg_loss = total_loss / training_data.len() as f64;
            if avg_loss < 0.001 {
                break;
            }
        }
    }

    pub fn predict(&mut self, input: &[f64]) -> Vec<f64> {
        self.forward(input)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_neuron_creation() {
        let neuron = Neuron::new(3, ActivationFunction::ReLU);
        assert_eq!(neuron.weights.len(), 3);
    }

    #[test]
    fn test_forward_pass() {
        let neuron = Neuron::new(2, ActivationFunction::Linear);
        let output = neuron.forward(&[1.0, 2.0]);
        assert!(output.is_finite());
    }

    #[test]
    fn test_network_creation() {
        let mut nn = NeuralNetwork::new(0.01);
        nn.add_layer(3, 5, ActivationFunction::ReLU);
        nn.add_layer(5, 1, ActivationFunction::Sigmoid);
        
        assert_eq!(nn.layers.len(), 2);
        assert_eq!(nn.layers[0].output_size, 5);
        assert_eq!(nn.layers[1].output_size, 1);
    }

    #[test]
    fn test_network_training() {
        let mut nn = NeuralNetwork::new(0.1);
        nn.add_layer(2, 4, ActivationFunction::ReLU);
        nn.add_layer(4, 1, ActivationFunction::Sigmoid);

        let training_data = vec![
            (vec![0.0, 0.0], vec![0.0]),
            (vec![0.0, 1.0], vec![1.0]),
            (vec![1.0, 0.0], vec![1.0]),
            (vec![1.0, 1.0], vec![0.0]),
        ];

        nn.train(&training_data, 100);

        let output = nn.predict(&[0.0, 0.0]);
        assert!(!output.is_empty());
    }
}
