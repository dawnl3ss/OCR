#include <stdlib.h>
#include <math.h>
#include "nxor_network.h"

static float sigmoid_function(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

static float sigmoid_derivative(float x)
{
    return x * (1.0f - x);
}

void initialize_network(NeuralNetwork *nn, int nb_inputs, int nb_hidden,
		int nb_output)
{
    nn->nb_inputs = nb_inputs;
    nn->nb_hidden = nb_hidden;
    nn->nb_output = nb_output;

    nn->hidden_layer_weights = malloc(nb_inputs * nb_hidden * sizeof(float));
    nn->output_layer_weights = malloc(nb_hidden * nb_output * sizeof(float));
    nn->hidden_layer_biases = malloc(nb_hidden * sizeof(float));
    nn->output_layer_bias = 0;

    for (int i = 0; i < nb_inputs * nb_hidden; i++) {
        nn -> hidden_layer_weights[i] = (float)rand()/RAND_MAX * 2.0f - 1.0f;
    }
    for (int i = 0; i < nb_hidden; i++) {
        nn -> hidden_layer_biases[i] = (float)rand()/RAND_MAX * 2.0f - 1.0f;
        nn -> output_layer_weights[i] = (float)rand()/RAND_MAX * 2.0f - 1.0f;
    }
}

float predict_output(NeuralNetwork *nn, float input[2]) {
    float hidden_layer_outputs[nn -> nb_hidden];
    float output;

    for (int i = 0; i < nn -> nb_hidden; i++) {
        hidden_layer_outputs[i] = nn -> hidden_layer_biases[i];
        for (int j = 0; j < nn -> nb_inputs; j++) {
            hidden_layer_outputs[i] +=
		input[j] * nn->hidden_layer_weights[i * nn -> nb_inputs + j];
        }
        hidden_layer_outputs[i] = sigmoid_function(hidden_layer_outputs[i]);
    }

    output = nn -> output_layer_bias;
    for (int i = 0; i < nn -> nb_hidden; i++) {
        output += hidden_layer_outputs[i] * nn -> output_layer_weights[i];
    }
    output = sigmoid_function(output);

    return 1.0f - output;
}

void train_neural_network(NeuralNetwork *nn, float input_samples[][2],
float expected_outputs[], int sample_count, int epochs, float learning_rate){
    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int sample = 0; sample < sample_count; sample++) {
            float *input = input_samples[sample];
            float expected_output = expected_outputs[sample];

            float hidden_layer_outputs[nn->nb_hidden];
            float output;

            for (int i = 0; i < nn->nb_hidden; i++) {
                hidden_layer_outputs[i] = nn->hidden_layer_biases[i];
                for (int j = 0; j < nn->nb_inputs; j++) {
                    hidden_layer_outputs[i] +=
		    input[j]*nn->hidden_layer_weights[i*nn->nb_inputs + j];
                }
                hidden_layer_outputs[i] =
			sigmoid_function(hidden_layer_outputs[i]);
            }

            output = nn->output_layer_bias;
            for (int i = 0; i < nn -> nb_hidden; i++) {
                output +=
		hidden_layer_outputs[i] * nn->output_layer_weights[i];
            }
            output = sigmoid_function(output);

            float output_error = expected_output - output;
            float output_delta = output_error * sigmoid_derivative(output);

            float hidden_deltas[nn->nb_hidden];
            for (int i = 0; i < nn->nb_hidden; i++) {
                hidden_deltas[i] = output_delta *
		nn->output_layer_weights[i] *
		sigmoid_derivative(hidden_layer_outputs[i]);
            }

            for (int i = 0; i < nn -> nb_hidden; i++)
            {
                nn->output_layer_weights[i] +=
		learning_rate * output_delta * hidden_layer_outputs[i];
            }
            nn->output_layer_bias += learning_rate * output_delta;

            for (int i = 0; i < nn -> nb_hidden; i++)
            {
                for (int j = 0; j < nn->nb_inputs; j++)
                {
                    nn->hidden_layer_weights[i * nn->nb_inputs + j] +=
			    learning_rate * hidden_deltas[i] * input[j];
                }
                nn->hidden_layer_biases[i] +=
			learning_rate * hidden_deltas[i];
            }
        }
    }
}

void free_neural_network(NeuralNetwork *nn)
{
    free(nn -> hidden_layer_weights);
    free( nn-> output_layer_weights);
    free( nn-> hidden_layer_biases);
}
