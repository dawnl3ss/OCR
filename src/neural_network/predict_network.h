#ifndef PREDICT_NETWORK_H
#define PREDICT_NETWORK_H

typedef struct {
    float* out_weights;
    float activation;
    float weighted_sum;
    float delta;
    float bias;
} NeuralNode;

typedef struct {
    NeuralNode* neu;
    int num_neurons;
} NeuralLayer;

extern int num_layers;
extern int* num_neurons;
extern NeuralLayer* lay;

void load_trained_network(const char* filename);
void forward_propagation();
char predict_with_network(float* input_data);
void softmax(float* input, float* output, int length);

#endif
