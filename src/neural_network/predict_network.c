#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "predict_network.h"

#define IMG_WIDTH 28
#define IMG_HEIGHT 28

int num_layers;
int* num_neurons;
NeuralLayer* lay;

void load_trained_network(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (!file)
    {
        printf("Error opening file for loading: %s\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == 0)
    {
        printf("Network state file is empty: %s\n", filename);
        fclose(file);
        return;
    }
    fseek(file, 0, SEEK_SET);

    fread(&num_layers, sizeof(int), 1, file);
    num_neurons = (int*) malloc(num_layers * sizeof(int));
    fread(num_neurons, sizeof(int), num_layers, file);

    lay = (NeuralLayer*) malloc(num_layers * sizeof(NeuralLayer));

    for (int i = 0; i < num_layers; ++i)
    {
        lay[i].neu = (NeuralNode*) malloc(num_neurons[i] * sizeof(NeuralNode));
        lay[i].num_neurons = num_neurons[i];
        if (i < num_layers - 1)
        {
            for (int j = 0; j < num_neurons[i]; ++j)
            {
                lay[i].neu[j].out_weights =(float*)malloc(num_neurons[i + 1] * 
                                                             sizeof(float));
            }
        }
    }

    for (int i = 0; i < num_layers - 1; ++i)
    {
        for (int j = 0; j < num_neurons[i]; ++j)
        {
            fread(lay[i].neu[j].out_weights, sizeof(float),
                                                    num_neurons[i + 1], file);
            fread(&lay[i].neu[j].bias, sizeof(float), 1, file);
        }
    }

    fclose(file);
    printf("Network loaded from %s\n", filename);
}

void forward_propagation()
{
    int i, j, k;
    for (i = 1; i < num_layers; i++)
    {   
        for (j = 0; j < num_neurons[i]; j++)
        {
            lay[i].neu[j].weighted_sum = lay[i].neu[j].bias;
            
            for (k = 0; k < num_neurons[i-1]; k++)
            {
                lay[i].neu[j].weighted_sum += lay[i-1].neu[k].activation * 
                                              lay[i-1].neu[k].out_weights[j];
            }
            
            if (i < num_layers - 1)
            {
                lay[i].neu[j].activation = 1.0 / (1.0 +
                                        exp(-lay[i].neu[j].weighted_sum));
            }
        }
    }

    float* output_activations = (float*) malloc(num_neurons[num_layers - 1] * 
                                                sizeof(float));
    for (j = 0; j < num_neurons[num_layers - 1]; j++)
    {
        output_activations[j] = lay[num_layers - 1].neu[j].weighted_sum;
    }
    softmax(output_activations,output_activations,num_neurons[num_layers - 1]);
    for (j = 0; j < num_neurons[num_layers - 1]; j++)
    {
        lay[num_layers - 1].neu[j].activation = output_activations[j];
    }
    free(output_activations);
}

char predict_with_network(float* input_data)
{
    for (int j = 0; j < num_neurons[0]; ++j) {
        lay[0].neu[j].activation = input_data[j];
    }

    forward_propagation();

    int predicted_label = 0;
    float max_output = lay[num_layers - 1].neu[0].activation;
    for (int j = 1; j < num_neurons[num_layers - 1]; ++j)
    {
        if (lay[num_layers - 1].neu[j].activation > max_output)
        {
            max_output = lay[num_layers - 1].neu[j].activation;
            predicted_label = j;
        }
    }

    char predicted_letter =  predicted_label + 'A';
    return predicted_letter;
}

void softmax(float* input, float* output, int length)
{
    float max = input[0];
    for (int i = 1; i < length; ++i)
    {
        if (input[i] > max)
        {
            max = input[i];
        }
    }

    float sum = 0.0;
    for (int i = 0; i < length; ++i)
    {
        output[i] = exp(input[i] - max);
        sum += output[i];
    }

    for (int i = 0; i < length; ++i)
    {
        output[i] /= sum;
    }
}