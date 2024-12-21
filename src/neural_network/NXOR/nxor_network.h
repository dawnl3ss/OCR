#ifndef NXOR_NETWORK_H
#define NXOR_NETWORK_H

typedef struct
{
    int nb_inputs;
    int nb_hidden;
    int nb_output;
    float *hidden_layer_weights;
    float *output_layer_weights;
    float *hidden_layer_biases;
    float output_layer_bias;
} NeuralNetwork;

void initialize_network(NeuralNetwork *nn, int nb_inputs, int nb_hidden,
		int nb_output);
void train_neural_network(NeuralNetwork *nn, float input_samples[][2],
		float expected_outputs[], int sample_count, int epochs,
		float learning_rate);
float predict_output(NeuralNetwork *nn, float input[2]);
void free_neural_network(NeuralNetwork *nn);

#endif
