#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "solver.h"

int findings(char** tableau,int j,int i,char word[],int p,int l,size_t len)
{
    int k = 2;
    while(abs(i)<len && abs(j)<len && tableau[p+j][l+i] == tolower(word[k]))
    {
        if(j > 0)
        {
            j++;
        }
        else if(j<0)
        {
            j--;
        }
        if(i>0)
        {
            i++;
        }
        else if(i<0)
        {
            i--;
        }
        k++;
    }
    if(abs(i)<len && i != 0 || abs(j)<len && j != 0 )
    {
        return 0;
    }
    return 1;

}


void Solver(char file_name[], char word[], int *X_prem, int *Y_prem, int *X_der, int *Y_der)
{
    int largeur=0;
    int profondeur=1;
    FILE* fichier = NULL;
    fichier = fopen(file_name, "r");
    int caract_actuel = 0;
    if (fichier != NULL)
    {
        int i = 0;
        int j = 0;
        int locked = 1;
        while(caract_actuel != EOF)
        {
            caract_actuel= fgetc(fichier);
            if(caract_actuel=='\n')
            {
                profondeur+=1;
                locked=0;
            }
            else
            {
                if(locked)
                {
                    largeur += 1;
                }
            }
        }
        rewind(fichier);
        char** tableau = malloc((profondeur)*sizeof(char*));
        for(int k=0; k<profondeur;k++)
        {
            tableau[k] = malloc(sizeof(char)*largeur);
        }
        caract_actuel = fgetc(fichier);
        while(caract_actuel != EOF)
        {
            if(caract_actuel=='\n')
            {
                j+=1;
                i=0;
            }
            else
            {
                tableau[j][i] = tolower(caract_actuel);
                i+=1;
            }
            caract_actuel = fgetc(fichier);
        }
        fclose(fichier);
        int is_found=0;
        int len = strlen(word);
        for(int p =0; p<profondeur;p++)
        {
            for(int l = 0 ;l<largeur;l++)
            {
                if(tableau[p][l] == tolower(word[0]))
                {
                    char next = tolower(word[1]);
                    if(l+len-1 < largeur && tableau[p][l+1] == next)
                    {
                        if(findings(tableau,0,2,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l+len-1;
                            *Y_prem = p;
                            *Y_der = p;
                            printf("(%i,%i)(%i,%i)\n",l,p,l+len-1,p);
                            is_found = 1;
                        }
                    }

                    if (l+len-1 < largeur && p+len-1 < profondeur &&
				    tableau[p+1][l+1] == next)
                    {
                        if(findings(tableau,2,2,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l+len-1;
                            *Y_prem = p;
                            *Y_der = p+len-1;
                            printf("(%i,%i)(%i,%i)\n",l,p,l+len-1,p+len-1);
                            is_found = 1;
                        }
                    }
                    if(p+len-1<profondeur&& tableau[p+1][l] == next)
                    {
                        if(findings(tableau,2,0,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l;
                            *Y_prem = p;
                            *Y_der = p+len-1;
                            printf("(%i,%i)(%i,%i)\n",l,p,l,p+len-1);
                            is_found = 1;
                        }
                    }
                    if(l-len+1 >= 0 && p+len-1 < profondeur &&
				    tableau[p+1][l-1] == next)
                    {
                        if(findings(tableau,2,-2,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l-len+1;
                            *Y_prem = p;
                            *Y_der = p+len-1;
                            printf("(%i,%i)(%i,%i)\n",l,p,l-len+1,p+len-1);
                            is_found = 1;
                        }
                    }
                    if(l-len+1 >=0 && tableau[p][l-1])
		    {
                        if(findings(tableau,0,-2,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l-len+1;
                            *Y_prem = p;
                            *Y_der = p;
                            printf("(%i,%i)(%i,%i)\n",l,p,l-len+1,p);
                            is_found = 1;
                        }
                    }
                    if(l-len+1 >= 0 && p-len+1 >=0 &&
				    tableau[p-1][l-1] == next)
                    {
                        if(findings(tableau,-2,-2,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l-len+1;
                            *Y_prem = p;
                            *Y_der = p-len+1;
                            printf("(%i,%i)(%i,%i)\n",l,p,l-len+1,p-len+1);
                            is_found = 1;
                        }
                    }
                    if(p-len+1>=0&& tableau[p-1][l] == next)
                    {
                        if(findings(tableau,-2,0,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l;
                            *Y_prem = p;
                            *Y_der = p-len+1;
                            printf("(%i,%i)(%i,%i)\n",l,p,l,p-len+1);
                            is_found = 1;
                        }
                    }
                    if(l+len-1 < largeur && p-len+1 >=0 &&
				    tableau[p-1][l-1] == next)
                    {
                        if(findings(tableau,-2,2,word,p,l,len))
                        {
                            *X_prem = l;
                            *X_der = l+len-1;
                            *Y_prem = p;
                            *Y_der = p-len+1;
                            printf("(%i,%i)(%i,%i)\n",l,p,l+len-1,p-len+1);
                            is_found = 1;
                        }
                    }
                }
            }
        }
        if(!is_found)
        {
            printf("Not found\n");
        }
    }
    else
    {
        printf("Impossible d'ouvrir le fichier %s",file_name);
    }
}

/*void main(){
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    Solver("grille.txt", "APPLE", &a, &b, &c, &d);
}*/