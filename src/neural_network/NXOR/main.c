#include <stdio.h>
#include "nxor_network.h"


int main()
{
    NeuralNetwork nn;
    initialize_network(&nn, 2, 4, 1);

    float input[4][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
    float expected_outputs[4] = { 0, 1, 1, 0 };

    train_neural_network(&nn, input, expected_outputs, 4, 10000, 0.5);

    printf("Résultats après entraînement :\n");
    for (int i = 0; i < 4; i++)
    {
        float output = predict_output(&nn, input[i]);
        printf("Entrées : %.0f, %.0f -> Sortie : %f\n", input[i][0],
			input[i][1], output);
    }

    free_neural_network(&nn);
    return 0;
}
