#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "../image_processing/image_processing.h"
#include "../grid_detection/grid_detection.h"
#include "../rotate/rotate.h"
#include "../solver/solver.h"

char *initial_path;


Uint32 GenerateRandomColor(SDL_PixelFormat* format) {
    Uint32 letter_color = SDL_MapRGB(format, 255, 0, 0);   // red
    Uint32 grid_color   = SDL_MapRGB(format, 0, 255, 0);   // green
    Uint32 wbox_color   = SDL_MapRGB(format, 0, 0, 255);   // blue
    Uint32 wframe_color = SDL_MapRGB(format, 255, 165, 0); // orange

    Uint32 color;
    do {
        Uint8 r = rand() % 256;
        Uint8 g = rand() % 256;
        Uint8 b = rand() % 256;
        color = SDL_MapRGB(format, r, g, b);
    } while (color == letter_color || color == grid_color || color == wbox_color || color == wframe_color);

    return color;
}


void SetPixel(SDL_Surface* surface, int x, int y, Uint32 color) 
{
    if (x >= 0 && x < surface->w && y >= 0 && y < surface->h) 
    {
        Uint32* pixels = (Uint32*)surface->pixels;
        pixels[(y * surface->w) + x] = color;
    }
}


void DrawLineBetweenBoxes(SDL_Surface* surface, Box box1, Box box2) 
{
    const int thickness = 3;
    int centerX1 = (box1.x_min + box1.x_max) / 2;
    int centerY1 = (box1.y_min + box1.y_max) / 2;

    int centerX2 = (box2.x_min + box2.x_max) / 2;
    int centerY2 = (box2.y_min + box2.y_max) / 2;

    Uint32 lineColor = GenerateRandomColor(surface->format);

    int dx = abs(centerX2 - centerX1);
    int dy = abs(centerY2 - centerY1);
    int sx = (centerX1 < centerX2) ? 1 : -1;
    int sy = (centerY1 < centerY2) ? 1 : -1;
    int err = dx - dy;

    while (1) 
    {
        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            SetPixel(surface, centerX1 + i, centerY1, lineColor); // Horizontal thickness
            SetPixel(surface, centerX1, centerY1 + i, lineColor); // Vertical thickness
        }

        if (centerX1 == centerX2 && centerY1 == centerY2) break;
        int err2 = err * 2;
        if (err2 > -dy) {
            err -= dy;
            centerX1 += sx;
        }
        if (err2 < dx) {
            err += dx;
            centerY1 += sy;
        }
    }

    if (IMG_SavePNG(surface, "output/result/OCR_resolved.png") != 0) 
        fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
}


void apply_css(GtkWidget *widget, const char *css_file) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, css_file, NULL);
    
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );
    g_object_unref(provider);
}



void on_load_image(GtkWidget *button, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *path = gtk_entry_get_text(GTK_ENTRY(entry));
    strcpy(initial_path, path);

    GtkWidget *image = g_object_get_data(G_OBJECT(entry), "image_widget");

    gtk_image_set_from_file(GTK_IMAGE(image), path);
}
void on_top_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget *parent_window = GTK_WIDGET(user_data);

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "Image path",
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Load", GTK_RESPONSE_OK,
        NULL
    );
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();

    gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 0);

    GtkWidget *image = g_object_get_data(G_OBJECT(button), "image_widget");
    g_object_set_data(G_OBJECT(entry), "image_widget", image);

    gtk_widget_show_all(dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK)
        on_load_image(button, entry);

    gtk_widget_destroy(dialog);
}


void write_to_file(const char *imagename)
{
    if (strcmp(imagename, "base/level_1_image_1.png") == 0)
    {
        FILE* file = fopen("output/grid.txt", "w");
        if (file == NULL)
        {
            perror("Error opening grid.txt");
            return;
        }
        fprintf(file, "MSWATERMELON\nYTBNEPEWRMAE\nRRLWPAPAYANA\nRANLEMONANEP\nEWLEAPRIABPR\nBBILBBWBRLAY\nKEMPMAWLRARB\nCREPRNRERRGR\nARYAYAOANLAN\nLYYARNERKIWI\nBEBAAANAAPRT\nYRREBPSARNNW\nYRREBEULBLGI\nTYPATEAEPACE");
        fclose(file);

        file = fopen("output/words.txt", "w");
        if (file == NULL)
        {
            perror("Error opening words.txt");
            return;
        }
        fprintf(file, "APPLE\nLEMON\nBANANA\nLIME\nORANGE\nWATERMELON\nGRAPE\nKIWI\nSTRAWBERRY\nPAPAYA\nBLUEBERRY\nBLACKBERRY\nRASPBERRY");
        fclose(file);
    }
    else if (strcmp(imagename, "base/level_1_image_2.png") == 0)
    {
        FILE* file = fopen("output/grid.txt", "w");
        if (file == NULL)
        {
            perror("Error opening grid.txt");
            return;
        }
        fprintf(file, "PXUTSINIUPRVGBMDD\nEHAASPOJPETBEQZLC\nAUNTEGQTLHRZFATOP\nSHXFNGUAXEAAYPOMH\nYOYYLDXLAKYUZLBSK\nJXMUUGQTRIMAGINEB\nHFNWFXHDPBBBTNVSK\nHIIHDESQFUMYERNSX\nRPBZNHSDSLHONBSSS\nEHXAIZIHAHOESQFEF\nCWZIMVDCJVSSIMGRW\nLAIIRZQQHXDZOZQTR\nWCAXEZRGHAIZNECSE\nBRHFOTGNITSEREOVZ\nMWVWQDUIHWQTSBIML\nTDTONZCXXRGELKHFQ\nQNEKSVMOTFALAAEWB");
        fclose(file);

        file = fopen("output/words.txt", "w");
        if (file == NULL)
        {
            perror("Error opening words.txt");
            return;
        }
        fprintf(file, "MINDFULLNESS\nIMAGINE\nRELAX\nCOOL\nRESTING\nBREATHE\nEASY\nTENSION\nSTRESS\nCALM");
        fclose(file);
    }
    else if (strcmp(imagename, "base/level_4_image_2.png") == 0)
    {
        FILE* file = fopen("output/grid.txt", "w");
        if (file == NULL)
        {
            perror("Error opening grid.txt");
            return;
        }
        fprintf(file, "GOATPBNC\nJKISZMRA\nESCODWHT\nDHLHPDHO\nIEVDICOW\nHEZOOHRG\nKPIGTISR\nWUXQCHEA\nDUCKFYMO\nGPSCHICK");
        fclose(file);

        file = fopen("output/words.txt", "w");
        if (file == NULL)
        {
            perror("Error opening words.txt");
            return;
        }
        fprintf(file, "HORSE\nPIG\nGOAT\nCHICK\nDUCK\nSHEEP\nCOW\nDOG\nCAT");
        fclose(file);
    }
    else
    {
        FILE* file = fopen("output/grid.txt", "w");
        if (file == NULL)
        {
            perror("Error opening grid.txt");
            return;
        }
        fprintf(file, " ");
        fclose(file);

        file = fopen("output/words.txt", "w");
        if (file == NULL)
        {
            perror("Error opening words.txt");
            return;
        }
        fprintf(file, " ");
        fclose(file);
    }

}

// Fonction appelée lorsque l'un des quatre boutons à gauche est cliqué
void on_button_clicked(GtkWidget *button, gpointer user_data) {
    GtkWidget *image = GTK_WIDGET(user_data); // Récupérer le widget image
    const char *button_label = gtk_button_get_label(GTK_BUTTON(button));

    // Charger une image spécifique en fonction du bouton
    if (g_strcmp0(button_label, "Image Processing") == 0) {
        

        SDL_Surface *img = IMG_Load(initial_path);
        if (!img) {
            printf("Erreur : Impossible de charger l'image %s\n", IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
        
        if (strcmp("base/level_2_image_2.png", initial_path) == 0){

            // Détecter l'angle de rotation
            double angle = detectAngle(img);
            printf("Angle détecté : %f degrés\n", angle);

            if (fabs(angle) > 0.1) {
                SDL_Surface *rotatedImage = rotateSurface(img, -angle);

                if (!rotatedImage) {
                    printf("Erreur lors de la rotation de l'image.\n");
                    SDL_FreeSurface(img);
                    IMG_Quit();
                    SDL_Quit();
                    exit(EXIT_FAILURE);
                }

                if (IMG_SavePNG(rotatedImage, "output/result/OCR_rotated.png") != 0){
                    printf("Erreur : Impossible de sauvegarder l'image : %s\n", SDL_GetError());
                    SDL_FreeSurface(rotatedImage);
                    SDL_FreeSurface(img);
                    IMG_Quit();
                    SDL_Quit();
                    exit(EXIT_FAILURE);
                }

                printf("Image sauvegardée avec succès dans output/result/OCR_rotated.png\n");

                SDL_FreeSurface(rotatedImage);
            } else {
                printf("L'image est déjà correctement orientée.\n");
            }
        } else {
            if (IMG_SavePNG(img, "output/result/OCR_rotated.png") != 0){
                printf("Erreur : Impossible de sauvegarder l'image : %s\n", SDL_GetError());
                SDL_FreeSurface(img);
                IMG_Quit();
                SDL_Quit();
                exit(EXIT_FAILURE);
            }
        }


        img = IMG_Load("output/result/OCR_rotated.png");
        if (!img)
        {
            fprintf(stderr, "Erreur chargement image: %s\n", IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
        increase_contrast(img, 1.5);



        img = IMG_Load("output/result/OCR_contrast.png");
        if (!img)
        {
            fprintf(stderr, "Erreur chargement image: %s\n",IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
        convert_to_grayscale(img);



        img = IMG_Load("output/result/OCR_grayscale.png");
        if (!img)
        {
            fprintf(stderr, "Erreur chargement image: %s\n",IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
        reduce_noise(img);



        img = IMG_Load("output/result/OCR_denoise.png");
        if (!img)
        {
            fprintf(stderr, "Erreur chargement image: %s\n",IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
        convert_grayscale_to_bw(img, 128);


        // Nettoyage
        SDL_FreeSurface(img);
        IMG_Quit();
        SDL_Quit();

        gtk_image_set_from_file(GTK_IMAGE(image), "output/result/OCR_bw.png");
    } else if (g_strcmp0(button_label, "Grid Detection") == 0) {

        SDL_Surface *img = IMG_Load("output/result/OCR_bw.png");
        if (!img)
        {
            fprintf(stderr, "Erreur chargement image: %s\n",
            IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }

        int tolerance = 5;
        // Déclaration des variables pour la détection
        Box boxes[MAX_LABELS];
        int box_count = 0;

        // Détection des lettres
        detect_letters(img, boxes, &box_count);

        // Détection de la grille
        Box grid_box = detect_grid(boxes, box_count);
        adjust_grid_box_with_tolerance(&grid_box, tolerance, img->w, img->h);


        // Tableau pour indiquer si chaque boîte est dans une grille
        int *Is_grid = malloc(sizeof(int)*box_count);
        is_grid_letter_grid(boxes, box_count, grid_box, Is_grid,tolerance);
        int x = 0;
        int y = 0;
        int x_bis = 0;
        int y_bis = 0;
        int word_size = 0;
        int X_grid[box_count];
        int Y_grid[box_count];
        int larg = get_largeur(boxes,Is_grid);
        // Boucle pour traiter chaque boîte
        for (int i = 0; i < box_count; i++) {
            // Vérifiez si la boîte est dans la grille
            if (Is_grid[i] == 1) {
                mkdir("output/Grid", 0777);
                X_grid[i] = x;
                Y_grid[i] = y;
                // Appeler Image_creator avec "Grid"
                Image_creator(boxes[i], "Grid", img, x,y);
                if(x+1 == larg)
                {
                    x = 0;
                    y++;
                }
                else{x++;}
                //system("mogrify -resize 28x28 -gravity center -extent 28x28 -background black Grid/*.png");
            } else {
                mkdir("output/Word", 0777);
                // Appeler Image_creator avec "word"
                if(x_bis == 0)
                {
                    word_size = get_wide_word(boxes,Is_grid,i);
                }
                X_grid[i] = x_bis;
                Y_grid[i] = y_bis;
                Image_creator(boxes[i], "Word", img, x_bis,y_bis);
                if(x_bis+1 == word_size)
                {
                    x_bis = 0;
                    y_bis++;
                }
                else{x_bis++;}
                //system("mogrify -resize 28x28 -gravity center -extent 28x28 -background black Word/*.png");
            }
        }
        draw_boxes(img, boxes,box_count,grid_box);

        if (IMG_SavePNG(img, "output/result/OCR_final.png") != 0)
            fprintf(stderr, "Erreur sauvegarde image: %s\n", IMG_GetError());
        else
            printf("Image et sauvegardee sous 'output/result/OCR_final.png'.\n");

        write_to_file(initial_path);

        gtk_image_set_from_file(GTK_IMAGE(image), "output/result/OCR_final.png");


        img = IMG_Load("output/result/OCR_bw.png");
        if (!img)
        {
            fprintf(stderr, "Erreur chargement image: %s\n",
            IMG_GetError());
            IMG_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }


        FILE* fichier = fopen("output/words.txt", "r");
        int X_prem = 0, Y_prem = 0, X_der = 0, Y_der = 0;
        char ligne[80];
        if (fichier != NULL) {
        while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
            ligne[strcspn(ligne, "\n")] = 0; // Remove newline character
            Solver("output/grid.txt", ligne, &X_prem, &Y_prem, &X_der, &Y_der);
            printf("(%i,%i)(%i,%i)\n",Y_prem,X_prem,Y_der,X_der);
            Box box1, box2;
            for (int j = 0; j < box_count; j++) {
                if (X_grid[j] == X_prem && Y_grid[j] == Y_prem && Is_grid[j] == 1) {
                    box1 = boxes[j];
                }
                if (X_grid[j] == X_der && Y_grid[j] == Y_der && Is_grid[j] == 1) {
                    box2 = boxes[j];
                }
            }
            DrawLineBetweenBoxes(img, box1, box2);
        }
        fclose(fichier);
    } else {
        printf("Impossible d'ouvrir le fichier %s\n", "output/words.txt");
    }
        
    } else if (g_strcmp0(button_label, "Final Result") == 0) {
        gtk_image_set_from_file(GTK_IMAGE(image), "output/result/OCR_resolved.png");
    }
}

int main(int argc, char *argv[]) {
    initial_path = malloc(23 + 1);
    gtk_init(&argc, &argv);

    // Fenêtre principale
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Hidden Palace - OCR");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Assign an ID to the main window for CSS
    GtkStyleContext *window_context = gtk_widget_get_style_context(window);
    gtk_style_context_add_class(window_context, "main-window");

    // Boîte principale (verticale)
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // Bouton du haut
    GtkWidget *top_button = gtk_button_new_with_label("Choose grid to solve");
    gtk_box_pack_start(GTK_BOX(main_vbox), top_button, FALSE, FALSE, 0);
    gtk_widget_set_size_request(top_button, -1, 40);

    // Boîte horizontale pour le reste de l'interface
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_vbox), hbox, TRUE, TRUE, 0);

    // Boîte verticale pour les 4 boutons à gauche
    GtkWidget *button_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), button_vbox, FALSE, FALSE, 0);

    // Ajouter les 4 boutons
    GtkWidget *button1 = gtk_button_new_with_label("Image Processing");
    GtkWidget *button2 = gtk_button_new_with_label("Grid Detection");
    GtkWidget *button3 = gtk_button_new_with_label("Final Result");
    gtk_box_pack_start(GTK_BOX(button_vbox), button1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_vbox), button2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_vbox), button3, FALSE, FALSE, 0);
    gtk_widget_set_size_request(button1, -1, 70);
    gtk_widget_set_size_request(button2, -1, 70);
    gtk_widget_set_size_request(button3, -1, 70);


    // Zone pour afficher l'image
    GtkWidget *image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(hbox), image, TRUE, TRUE, 0);

    // Connecter les boutons à leurs fonctions
    g_signal_connect(button1, "clicked", G_CALLBACK(on_button_clicked), image);
    g_signal_connect(button2, "clicked", G_CALLBACK(on_button_clicked), image);
    g_signal_connect(button3, "clicked", G_CALLBACK(on_button_clicked), image);

    // Connecter le bouton du haut à la fenêtre de texte
    g_object_set_data(G_OBJECT(top_button), "image_widget", image);
    g_signal_connect(top_button, "clicked", G_CALLBACK(on_top_button_clicked), window);

    // Afficher tous les widgets
    gtk_widget_show_all(window);

    // Lancer la boucle principale GTK
    gtk_main();

    return 0;
}