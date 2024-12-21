    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>

    #include "train_network.h"

    layer* lay;
    int num_layers;
    int* num_neurons;
    float learning_rate = 0.0001;
    float** input;
    int* labels;
    int num_training_tests;
    int num_epochs = 500;

    int main()
    {
        int i, j;
        const char* network_file = "network_state.bin";

        srand(time(0));

        num_layers = 4; 

        num_neurons = (int*) malloc(num_layers * sizeof(int));
        memset(num_neurons, 0, num_layers * sizeof(int));

        num_neurons[0] = IMG_WIDTH * IMG_HEIGHT;
        num_neurons[1] = 128;
        num_neurons[2] = 64;
        num_neurons[3] = 26;

        if (initialize() != SUCCESS_INIT)
        {
            printf("Error in Initialization...\n");
            exit(0);
        }


        load_network(network_file);

        
        num_training_tests = 26*170;

        input = (float**) malloc(num_training_tests * sizeof(float*));
        labels = (int*) malloc(num_training_tests * sizeof(int));
        for (i = 0; i < num_training_tests; ++i)
        {
            input[i] = (float*) malloc(num_neurons[0] * sizeof(float));
        }

        for (i = 0; i < num_training_tests; ++i)
        {
            char filename[256];
            sprintf(filename, "dataset/%c%04d.png", 'A' + (i / 170), i % 170);
            load_image(filename, input[i], IMG_WIDTH, IMG_HEIGHT);
            labels[i] = get_label_from_filename(filename);
        }

        
        

        for (int epoch = 0; epoch < num_epochs; ++epoch)
    {
        for (i = 0; i < num_training_tests; ++i)
        {
            char filename[256];
            sprintf(filename, "dataset/%c%04d.png", 'A' + (i / 170), i % 170);
            SDL_Surface* img = IMG_Load(filename);
            if (!img)
            {
                printf("Unable to load image %s! SDL_image Error: %s\n", 
                                                    filename, IMG_GetError());
                continue;
            }


            for (j = 0; j < num_neurons[0]; ++j)
            {
                lay[0].neu[j].actv = input[i][j];
            }

            forward_prop();


            calculate_loss_and_backprop(labels[i]);


            update_weights();


        
        }
            save_network(network_file);
        }
        
        printf("Evaluating network...\n");
        train();

        for (i = 0; i < num_training_tests; ++i)
        {
            free(input[i]);
        }
        free(input);
        free(labels);
        free(num_neurons);

        return 0;
    }

    void load_image(const char* filename, float* input_data, int width,
                                                                int height)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL_Error: %s\n",SDL_GetError());
            exit(1);
        }

        SDL_Surface* img = IMG_Load(filename);
        if (!img)
        {
            printf("Unable to load image %s! SDL_image Error: %s\n", filename,
                                                            IMG_GetError());
            SDL_Quit();
            exit(1);
        }

        SDL_Surface* resized_img = SDL_CreateRGBSurface(0, width, height, 32,
                            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_BlitScaled(img, NULL, resized_img, NULL);

        Uint32* pixels = (Uint32*)resized_img->pixels;
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                Uint32 pixel = pixels[i * width + j];
                Uint8 r, g, b;
                SDL_GetRGB(pixel, resized_img->format, &r, &g, &b);
                input_data[i * width + j] = (r + g + b) / (3.0 * 255.0);
            }
        }

        SDL_FreeSurface(img);
        SDL_FreeSurface(resized_img);
        SDL_Quit();
    }

    int get_label_from_filename(const char* filename)
    {
        const char* base_name = strrchr(filename, '/');
        if (!base_name)
        {
            base_name = filename;
        }
        else
        {
            base_name++;
        }

        char label_char = base_name[0];

        if (label_char >= 'A' && label_char <= 'Z')
        {
            return label_char - 'A';
        }
        else
        {
            printf("Invalid label character in filename: %s\n", filename);
            exit(1);
        }
    }

    void forward_prop()
    {
        int i, j, k;
        for (i = 1; i < num_layers; i++)
        {   
            for (j = 0; j < num_neurons[i]; j++)
            {
                lay[i].neu[j].dactv = lay[i].neu[j].bias;
                
                for (k = 0; k < num_neurons[i-1]; k++)
                {
                    lay[i].neu[j].dactv += lay[i-1].neu[k].actv *
                                                lay[i-1].neu[k].out_weights[j];
                }
                
                if (i < num_layers - 1)
                {
                    lay[i].neu[j].actv = 1.0 / (1.0+exp(-lay[i].neu[j].dactv));
                }
            }
        }

        float* output_activations = (float*) malloc(num_neurons[num_layers - 1]
                                                            * sizeof(float));
        for (j = 0; j < num_neurons[num_layers - 1]; j++)
        {
            output_activations[j] = lay[num_layers - 1].neu[j].dactv;
        }
        softmax(output_activations, output_activations,
                                                num_neurons[num_layers - 1]);
        for (j = 0; j < num_neurons[num_layers - 1]; j++)
        {
            lay[num_layers - 1].neu[j].actv = output_activations[j];
        }
        free(output_activations);
    }

    void calculate_loss_and_backprop(int label) {
        int i, j, k;
        float loss = 0.0;

        for (j = 0; j < num_neurons[num_layers - 1]; ++j) {
            float target = (j == label) ? 1.0 : 0.0;
            float output = lay[num_layers - 1].neu[j].actv;
            if (target == 1.0) {
                loss -= log(output);
            }
            lay[num_layers - 1].neu[j].delta = output - target;
        }
        printf("Loss: %f\n", loss);

        for (i = num_layers - 2; i >= 0; --i) {
            for (j = 0; j < num_neurons[i]; ++j) {
                lay[i].neu[j].dactv = 0.0;
                for (k = 0; k < num_neurons[i + 1]; ++k) {
                    lay[i].neu[j].dactv += lay[i + 1].neu[k].delta
                                                * lay[i].neu[j].out_weights[k];
                }
                lay[i].neu[j].delta = lay[i].neu[j].dactv *
                            (lay[i].neu[j].actv * (1 - lay[i].neu[j].actv));
            }
        }
    }

    void update_weights()
    {
        int i, j, k;

        for (i = 0; i < num_layers - 1; ++i)
        {
            for (j = 0; j < num_neurons[i]; ++j)
            {
                for (k = 0; k < num_neurons[i + 1]; ++k)
                {
                    lay[i].neu[j].out_weights[k] -= learning_rate *
                                    lay[i + 1].neu[k].delta * lay[i].neu[j].actv;
                }
                lay[i].neu[j].bias -= learning_rate * lay[i].neu[j].delta;
            }
        }
    }

    void train()
    {
        int i, j, correct_predictions = 0;
        int num_test_ex = 26 * 170;
        float** test_input;
        int* test_labels;

        test_input = (float**) malloc(num_test_ex * sizeof(float*));
        test_labels = (int*) malloc(num_test_ex * sizeof(int));
        for (i = 0; i < num_test_ex; ++i)
        {
            test_input[i] = (float*) malloc(num_neurons[0] * sizeof(float));
        }

        for (i = 0; i < num_test_ex; ++i)
        {
            char filename[256];
            sprintf(filename, "dataset/%c%04d.png", 'A' + (i / 170), i % 170);
            load_image(filename, test_input[i], IMG_WIDTH, IMG_HEIGHT);
            test_labels[i] = get_label_from_filename(filename);
        }

        for (i = 0; i < num_test_ex; ++i)
        {
            for (j = 0; j < num_neurons[0]; ++j)
            {
                lay[0].neu[j].actv = test_input[i][j];
            }

            forward_prop();

            int predicted_label = 0;
            float max_output = lay[num_layers - 1].neu[0].actv;
            for (j = 1; j < num_neurons[num_layers - 1]; ++j)
            {
                if (lay[num_layers - 1].neu[j].actv > max_output)
                {
                    max_output = lay[num_layers - 1].neu[j].actv;
                    predicted_label = j;
                }
            }

            if (predicted_label == test_labels[i])
            {
                correct_predictions++;
            }
            else
            {
                printf("Predicted: %c, Actual: %c\n", predicted_label + 'A',
                                                        test_labels[i] + 'A');
            }
        }

        float accuracy = (float)correct_predictions / num_test_ex * 100.0;
        printf("Accuracy: %.2f%%\n", accuracy);

        for (i = 0; i < num_test_ex; ++i)
        {
            free(test_input[i]);
        }
        free(test_input);
        free(test_labels);
    }

    int initialize()
    {
        if(create_architecture() != SUCCESS_CREATE_ARCHITECTURE)
        {
            printf("Error in creating architecture...\n");
            return ERR_INIT;
        }

        printf("Neural Network Created Successfully...\n\n");
        return SUCCESS_INIT;
    }

    int create_architecture() {
        int i = 0;

        lay = (layer*) malloc(num_layers * sizeof(layer));
        if (lay == NULL) {
            printf("Memory allocation failed for layers\n");
            exit(1);
        }

        for (i = 0; i < num_layers; i++) {
            int next_layer_neurons = (i < num_layers - 1)?num_neurons[i+1]: 0;
            lay[i] = create_layer(num_neurons[i], next_layer_neurons);
            lay[i].num_neu = num_neurons[i];
            printf("Created Layer: %d\n", i + 1);
        }

        return SUCCESS_CREATE_ARCHITECTURE;
    }

    layer create_layer(int num_neurons, int next_layer_neurons) {
        layer l;
        l.num_neu = num_neurons;
        l.neu = (neuron*) malloc(num_neurons * sizeof(neuron));
        if (l.neu == NULL) {
            printf("Memory allocation failed for neurons\n");
            exit(1);
        }

        initialize_weights(&l, num_neurons, next_layer_neurons);

        printf("Created layer with %d neurons\n", num_neurons);
        for (int i = 0; i < num_neurons; ++i) {
            printf("Neuron %d - Bias: %f, Out_weights: %p\n", i, l.neu[i].bias,
                                                (void*)l.neu[i].out_weights);
        }

        return l;
    }

    void save_network(const char* filename)
    {
        FILE* file = fopen(filename, "wb");
        if (!file)
        {
            printf("Error opening file for saving: %s\n", filename);
            return;
        }

        fwrite(&num_layers, sizeof(int), 1, file);
        fwrite(num_neurons, sizeof(int), num_layers, file);

        for (int i = 0; i < num_layers - 1; ++i)
        {
            for (int j = 0; j < num_neurons[i]; ++j)
            {
                fwrite(lay[i].neu[j].out_weights, sizeof(float),
                                                    num_neurons[i + 1], file);
                fwrite(&lay[i].neu[j].bias, sizeof(float), 1, file);
            }
        }

        fclose(file);
    }

    void load_network(const char* filename)
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

        lay = (layer*) malloc(num_layers * sizeof(layer));

        for (int i = 0; i < num_layers; ++i)
        {
            lay[i].neu = (neuron*) malloc(num_neurons[i] * sizeof(neuron));
            lay[i].num_neu = num_neurons[i];
            if (i < num_layers - 1)
            {
                for (int j = 0; j < num_neurons[i]; ++j)
                {
                    lay[i].neu[j].out_weights = (float*) 
                                    malloc(num_neurons[i + 1] * sizeof(float));
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

    void augment_image(SDL_Surface* img, float* input_data, int width, int height)
    {
        SDL_Surface* augmented_img = SDL_CreateRGBSurface(0, width, height,32, 
                                0x00FF0000,0x0000FF00, 0x000000FF, 0xFF000000);

        double angle = (rand() % 360) - 180;
        SDL_Point center = {width / 2, height / 2};
        SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(augmented_img);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
        SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, &center, SDL_FLIP_NONE);

        Uint32* pixels = (Uint32*)augmented_img->pixels;
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                Uint32 pixel = pixels[i * width + j];
                Uint8 r, g, b;
                SDL_GetRGB(pixel, augmented_img->format, &r, &g, &b);
                input_data[i * width + j] = (r + g + b) / (3.0 * 255.0);
            }
        }

        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_FreeSurface(augmented_img);
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

    void print_weights_and_biases() {
        printf("\nWeights and Biases:\n");
        for (int i = 0; i < num_layers - 1; ++i) {
            printf("Layer %d:\n", i);
            for (int j = 0; j < num_neurons[i]; ++j) {
                printf("Neuron %d - Bias: %f\n", j, lay[i].neu[j].bias);
                printf("Weights: ");
                for (int k = 0; k < num_neurons[i + 1]; ++k) {
                    printf("%f ", lay[i].neu[j].out_weights[k]);
                }
                printf("\n");
            }
        }
    }

    void print_layer_activations() {
        printf("\nLayer Activations:\n");
        for (int i = 0; i < num_layers; ++i) {
            printf("Layer %d:\n", i);
            for (int j = 0; j < num_neurons[i]; ++j) {
                printf("%f ", lay[i].neu[j].actv);
            }
            printf("\n");
        }
    }

    void print_gradients() {
        printf("\nGradients:\n");
        for (int i = 0; i < num_layers; ++i) {
            printf("Layer %d:\n", i);
            for (int j = 0; j < num_neurons[i]; ++j) {
                printf("Neuron %d - delta: %f, dactv: %f\n", j, lay[i].neu[j].delta,
                                                        lay[i].neu[j].dactv);
            }
        }
    }

    void initialize_weights(layer* l, int num_neurons, int next_layer_neurons) {
        for (int i = 0; i < num_neurons; ++i) {
            l->neu[i].out_weights = (float*) malloc(next_layer_neurons * sizeof(float));
            for (int j = 0; j < next_layer_neurons; ++j) {
                l->neu[i].out_weights[j] = ((float)rand() / RAND_MAX) * 0.01;
            }
            l->neu[i].bias = ((float)rand() / RAND_MAX) * 0.01;
        }
    }