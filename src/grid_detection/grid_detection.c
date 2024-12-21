#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "grid_detection.h"

#define MAX_LABELS 10000         //limit of number of letters (composantes connexes)
#define SPACING_THRESHOLD 4      //spacing threshold between letters in grid


//detect letters using connex components
int is_white(Uint8 r, Uint8 g, Uint8 b) {
    return (r >= 255 && g >= 255 && b >= 255); // Returns 1 if the pixel is white
}

//detect letters using connex components
void detect_letters(SDL_Surface *image, Box *boxes, int *box_count){
    int width = image->w;
    int height = image->h;
    Uint32 *pixels = image->pixels;

    int labels[height][width];
    int visited = 1;

    //initialize all labels to 0 = no pixel visited yet
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            labels[y][x] = 0;
        }
    }

    *box_count = 0;

    //going accross image to find connexe components
    for(int y = 0; y < height; y++){
        for(int x= 0; x < width; x++){

            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * width +x], image->format, &r,&g,&b);//cur pixel rgb

            //if white and not marked
            if(is_white(r, g, b) && labels[y][x] == 0){

                if(*box_count >= MAX_LABELS){
                    printf("Erreur : trop de lettres détectées > MAX_LABELS.\n");
                    return;
                }


                Box box = {x, y, x, y};//new box creation
                labels[y][x] = visited; //pixel visited

                //stack for searching connex pixels
                int stack_size = 1;
                int stack_x[MAX_LABELS] = {x};
                int stack_y[MAX_LABELS] = {y};
                int nbr_pixel_blanc = 1;

                while(stack_size > 0){

                    //current pixel
                    int cx = stack_x[--stack_size];
                    int cy = stack_y[stack_size];

                    //update box limits
                    if(cx < box.x_min){box.x_min = cx;}
                    if(cy < box.y_min){box.y_min = cy;}

                    if(cx > box.x_max){box.x_max = cx;}
                    if(cy > box.y_max){box.y_max = cy;}

                    //exploring the 8 neighbors
                    for(int dy= -1; dy <= 1;dy++){//each line starting from the bottom
                        for(int dx = -1;dx <= 1; dx++){

                            //neighbor pixel
                            int nx = cx+dx;
                            int ny = cy+dy;

                            //verify image limits and visited pixel where dy=dx=0
                            if(nx >= 0 && nx < width && ny >= 0 && ny < height && labels[ny][nx] == 0){

                                Uint8 nr, ng, nb;
                                SDL_GetRGB(pixels[ny * width + nx], image->format, &nr, &ng, &nb);

                                //if neighbor white add to the stack
                                if(is_white(nr, ng, nb)){
                                    labels[ny][nx] = visited;
                                    stack_x[stack_size] = nx;
                                    stack_y[stack_size] = ny;
                                    stack_size++;
                                    nbr_pixel_blanc++;


                                    if(stack_size >=MAX_LABELS){
                                        printf("Erreur : trop de pixels dans une lettre.\n");
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }


                int box_width = box.x_max - box.x_min;
                int box_height = box.y_max - box.y_min;


                //add the box to boxes if right size
                if(box_width * box_height > 9  && box_width * box_height < 900  && nbr_pixel_blanc >= 10){//minimum surface to avoid noise, should add a max?
                    boxes[*box_count] = box;
                    (*box_count)++;
                }
                visited++;
            }
        }
    }
}

void Image_creator(Box box, const char *dossier, SDL_Surface *image, int x,int y)
{
		int imgw = box.x_max - box.x_min+1;
        int imgh = box.y_max - box.y_min+1;

        //new surface for new picture
        SDL_Surface *imgletter = SDL_CreateRGBSurface(0, imgw, imgh, image->format->BitsPerPixel,
                                                       image->format->Rmask, image->format->Gmask,
                                                       image->format->Bmask, image->format->Amask);
        if(imgletter == NULL){

          printf("Erreur : impossible de créer la surface pour la lettre.\n");
        }

        //copy of leetter in new picture
        for(int y = 0; y < imgh; y++){
            for(int x =0; x < imgw;x++){

                Uint32 *ipixel = image->pixels;
                Uint32 *nipixel =imgletter->pixels;

                Uint32 pixel = ipixel[(box.y_min + y) * image->w + (box.x_min + x)];
                nipixel[y * imgletter->w + x] = pixel;
            }
        }



        //create filname for letter
        char filename[50];
        snprintf(filename, sizeof(filename), "output/%s/%02d-%02d.png",dossier,y,x);

        //save image
        if(IMG_SavePNG(imgletter, filename) != 0) {
            fprintf(stderr, "Erreur : impossible de sauvegarder l'image de la lettre en PNG. %s\n", IMG_GetError());
        }

        // Libérer la surface de la lettre
        SDL_FreeSurface(imgletter);
}



int get_largeur(Box *boxes, int *Is_grid)
{
    int larg = 1;
    int lock = 1;
    int i = 0;
    while(!Is_grid[i])
    {
        i++;
    }
    int y_start = boxes[i].y_min;
    while(lock)
    {
        i++;
        if(Is_grid[i])
        {
            if( abs(y_start - boxes[i].y_min)  <= SPACING_THRESHOLD)
            {
                larg++;
            }
            else{lock = 0;}
        }

    }
    return larg;
}

int get_wide_word(Box *boxes, int *Is_grid,int i)
{
    int larg = 1;
    int lock = 1;
    int y_start = boxes[i].y_min;
    int x=boxes[i].x_max;
    while(lock)
    {
        i++;
        if( abs(y_start - boxes[i].y_min) <= SPACING_THRESHOLD && abs(x - boxes[i].x_min) <= SPACING_THRESHOLD)
        {
            larg++;
            x = boxes[i].x_max;
        }
        else{lock = 0;}
    }
    return larg;
}



//detecting the grid
Box detect_grid(Box *boxes, int box_count){

    //first box coordinates
    int x_min = boxes[0].x_min;
    int y_min = boxes[0].y_min;
    int x_max = boxes[0].x_max;
    int y_max = boxes[0].y_max;

    //going through all boxes
    for(int i= 1; i < box_count;i++){


        int current_x_min = boxes[i].x_min;
        int current_y_min = boxes[i].y_min;

        //try looking for min and max among boxes to draw grid
        if(abs(current_x_min - x_min) <=SPACING_THRESHOLD || abs(current_y_min - y_min) <= SPACING_THRESHOLD){ //should add max ? and should be less for y ?


            if(boxes[i].x_min < x_min){x_min = boxes[i].x_min;}
            if(boxes[i].y_min < y_min){y_min = boxes[i].y_min;}

            if(boxes[i].x_max > x_max){x_max = boxes[i].x_max;}
            if(boxes[i].y_max > y_max){y_max = boxes[i].y_max;}
        }




    }

    Box grid_box = {x_min, y_min, x_max, y_max};
    return grid_box;

}
void adjust_grid_box_with_tolerance(Box *grid_box, int tolerance, int image_width, int image_height) {
    grid_box->x_min = (grid_box->x_min - tolerance > 0) ? grid_box->x_min - tolerance : 0;
    grid_box->y_min = (grid_box->y_min - tolerance > 0) ? grid_box->y_min - tolerance : 0;
    grid_box->x_max = (grid_box->x_max + tolerance < image_width) ? grid_box->x_max + tolerance : image_width - 1;
    grid_box->y_max = (grid_box->y_max + tolerance < image_height) ? grid_box->y_max + tolerance : image_height - 1;
}

void is_grid_letter_grid(Box *boxes, int box_count, Box grid_box, int *Is_grid, int tolerance)
{
	size_t grid_number_of_boxes=0;
	size_t rest_number_of_boxes=0;
	int g_x_min = grid_box.x_min;
	int g_x_max = grid_box.x_max;
	int g_y_min = grid_box.y_min;
	int g_y_max = grid_box.y_max;
	for(int i = 0; i< box_count; i++)
	{
		int x_min = boxes[i].x_min + tolerance;
		int x_max = boxes[i].x_max - tolerance;
		int y_min = boxes[i].y_min + tolerance;
		int y_max = boxes[i].y_max - tolerance;

		if(x_min>=g_x_min && x_max<=g_x_max && y_min>=g_y_min && y_max<=g_y_max)
		{
			grid_number_of_boxes++;
		}
		else{rest_number_of_boxes++;}

	}
	int _is_grid = -1;
	if(grid_number_of_boxes > rest_number_of_boxes)
	{
		_is_grid = 1;
	}
	for(int i = 0; i< box_count; i++)
	{
		int x_min = boxes[i].x_min;
		int x_max = boxes[i].x_max;
		int y_min = boxes[i].y_min;
		int y_max = boxes[i].y_max;

		if(x_min>=g_x_min && x_max<=g_x_max && y_min>=g_y_min && y_max<=g_y_max)
		{
			Is_grid[i] = _is_grid;
		}
		else{Is_grid[i] = -_is_grid;}
	}
	return;
}


//drawing function
void draw_boxes(SDL_Surface *image, Box *boxes, int box_count, Box grid_box) {
    //pixzel colors
    Uint32 letter_color = SDL_MapRGB(image->format, 255,   0,   0);   //red
    Uint32 grid_color   = SDL_MapRGB(image->format,   0, 255,   0);   //green
    Uint32 wbox_color   = SDL_MapRGB(image->format,   0,   0, 255);   //blue
    Uint32 wframe_color = SDL_MapRGB(image->format, 255, 165,   0);   //orange

    Uint32 *pixels = image->pixels;

    //blue box coord
    int x_min = -1;
    int y_min = -1;
    int x_max = -1;
    int y_max = -1;

    //draw red lines et déterminer les limites des lettres hors de la grille
    for(int i =0; i < box_count;i++){

        Box box= boxes[i];

        // Dessiner toutes les lettres en rouge
        for(int x= box.x_min; x <=box.x_max;x++){

            //top line
            pixels[box.y_min * image->w + x] = letter_color;
            //bottom line
            pixels[box.y_max * image->w + x] = letter_color;
        }

        for(int y= box.y_min; y <= box.y_max;y++){

            //left line
            pixels[y * image->w + box.x_min] = letter_color;
            //right line
            pixels[y * image->w + box.x_max] = letter_color;
        }



        //check if box is outside the green grid
        if(box.x_max < grid_box.x_min || box.x_min > grid_box.x_max ||
            box.y_max < grid_box.y_min || box.y_min > grid_box.y_max){ //should add tolerence?

            //blue grid limits
            if(x_min == -1 || box.x_min < x_min){x_min = box.x_min;}
            if(y_min == -1 || box.y_min < y_min){y_min = box.y_min;}
            if(x_max == -1 || box.x_max > x_max){x_max = box.x_max;}
            if(y_max == -1 || box.y_max > y_max){y_max = box.y_max;}
        }



    }



    //draw green grid
    for(int x= grid_box.x_min; x <= grid_box.x_max; x++){

        //top line
        pixels[grid_box.y_min * image->w + x] = grid_color;
        //bottom line
        pixels[grid_box.y_max * image->w + x] = grid_color;
    }

    for(int y= grid_box.y_min; y<= grid_box.y_max;y++){
        //left line
        pixels[y * image->w + grid_box.x_min] = grid_color;
        //right line
        pixels[y * image->w + grid_box.x_max] = grid_color;
    }

    //draw blue box
    if(x_min != -1 && y_min != -1){//check if letters outside grid exist

        for(int x= x_min; x <= x_max; x++){

            //top line
            pixels[y_min * image->w + x] = wbox_color;
            //bottom line
            pixels[y_max * image->w + x] = wbox_color;
        }

        for(int y= y_min; y <= y_max; y++){

            //left line
            pixels[y * image->w + x_min] = wbox_color;
            //right line
            pixels[y * image->w + x_max] = wbox_color;
        }
    }


    //draw orange boxes
    for(int w_y= y_min; w_y <= y_max;w_y++){//going through blue box vertically

        int wordx_min = image->w, wordx_max = 0;
        int wordy_min = image->h, wordy_max = 0;

        //going through boxes
        for(int i= 0; i < box_count; i++){

            Box box = boxes[i];

            //check if oustide of green gride and inside blue box
            if(box.y_min >= w_y && box.y_min <= w_y + 1 &&
                (box.x_max < grid_box.x_min || box.x_min > grid_box.x_max)){

                //update limits
                if(box.x_min < wordx_min){wordx_min = box.x_min;}
                if(box.x_max > wordx_max){wordx_max = box.x_max;}

                if(box.y_min < wordy_min){wordy_min = box.y_min;}
                if(box.y_max > wordy_max){wordy_max = box.y_max;}
            }



        }

        //draw orange lines if word found
        if(wordx_min < wordx_max && wordy_min < wordy_max){

            for(int x = wordx_min; x <= wordx_max; x++){

                pixels[wordy_min * image->w + x] = wframe_color;
                pixels[wordy_max * image->w + x] = wframe_color;
            }

            for(int y= wordy_min; y <= wordy_max;y++){

                pixels[y * image->w + wordx_min] = wframe_color;
                pixels[y * image->w + wordx_max] = wframe_color;
            }
        }
    }
}