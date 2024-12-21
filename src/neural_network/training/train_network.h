#ifndef TRAIN_NETWORK_H
#define TRAIN_NETWORK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define IMG_WIDTH 28
#define IMG_HEIGHT 28
#define SUCCESS_INIT 0
#define ERR_INIT -1
#define SUCCESS_CREATE_ARCHITECTURE 0
#define ERR_CREATE_ARCHITECTURE -1

typedef struct {
    float* out_weights;
    float activation;
    float dactv;
    float delta;
    float bias;
} neuron;

typedef struct {
    neuron* neu;
    int num_neu;
} layer;

extern layer* lay;
extern int num_layers;
extern int* num_neurons;
extern float learning_rate;
extern float** input;
extern int* labels;
extern int num_training_tests;
extern int nb_epochs;

void load_image(const char* filename, float* input_data, int width,int height);
int get_label_from_filename(const char* filename);
void forward_prop();
void calculate_loss_and_backprop(int label);
void update_weights();
void train();
int initialize();
int create_architecture();
layer create_layer(int num_neurons, int next_layer_neurons);
void save_network(const char* filename);
void load_network(const char* filename);
void augment_image(SDL_Surface* img, float* input_data, int width, int height);
void softmax(float* input, float* output, int length);
void print_weights_and_biases();
void print_layer_activations();
void print_gradients();
void initialize_weights(layer* l, int num_neurons, int next_layer_neurons);

#endif