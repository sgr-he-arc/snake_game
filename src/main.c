//Spécifie à SDL que le projet est en console sous main
# define SDL_MAIN_HANDLED

//Includes systèmes
# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <conio.h>
# include <signal.h>
# include <windows.h>
# include <string.h>

//includes de librairies externes
# include "SDL.h"
# include "SDL_ttf.h"
# include "cJSON.h"

//includes persos, liens avec d'autres fichiers
# include "keys_mapping_json.h"
# include "remapping.h"

//méthodes pour quitter le programme
void QuitProgram(void) {
    SDL_Quit();
    TTF_Quit();
    exit(0);
}
void QuitError(void) {
    SDL_Quit();
    TTF_Quit();
    exit(1);
}

//logique pour les commandes
typedef struct {
    char *name;
    char *desc;
    void (*function)(void);
} Command;

Command commands[];

void help(void) {
    system("cls");
    for (size_t i = 0; commands[i].name != NULL; i++)
    {
        printf("/%s:\t%s\n", commands[i].name,commands[i].desc);
    }
}

void config(void) {
    if (config_mapping_view()) {
        printf("An error occured while creating remapping view\n");
        QuitError();
    }
}

Command commands[] = {
    {"help", "shows the available commands", help},
    {"config", "opens a window to change keybinding", config},
    {NULL, NULL, NULL}
};


int main(void) {
    //Initialisation des outils graphiques
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        QuitError();
    }

    
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        QuitError();
    }

    //récupération des touches
    if (read_config_file()) {
        printf("An error occured, please check the config file");
        getch();
        QuitError();
    }

    //Projet console
    const int game_board_size = 17;
    bool game_board[game_board_size][game_board_size];
    for (int i = 0; i < game_board_size; i++)
    {
        for (int j = 0; j < game_board_size; j++)
        {
            game_board[i][j] = 0;
        }
        
    }

    int coordX = (int)(game_board_size / 2);
    int coordY = (int)(game_board_size / 2);
    
    int userInput = 0;

    while (true)
    {
        game_board[coordY][coordX] = 1;
        system("cls");
        for (int i = 0; i < game_board_size; i++)
        {
            for (int j = 0; j < game_board_size; j++)
            {
                if (game_board[i][j]) {
                    printf("*");
                }
                else {
                printf(" ");
                }
            }
            printf("\n");
        }
        game_board[coordY][coordX] = 0;
        userInput = getch();
        if (userInput == 224) {
            userInput = getch();
        }
        if (userInput == game_keys[0].key) {
            coordY--;
            if (coordY < 0) {
                coordY += game_board_size;
            }
        } else if (userInput == game_keys[1].key) {
            coordY++;
            if (coordY > game_board_size - 1) {
                coordY -= game_board_size;
            }
        } else if (userInput == game_keys[2].key) {
            coordX--;
            if (coordX < 0) {
                coordX += game_board_size;
            }
        } else if (userInput == game_keys[3].key) {
            coordX++;
            if (coordX > game_board_size - 1) {
                coordX -= game_board_size;
            }
        } else if (userInput == game_keys[4].key) {
            char userInputChar = 0x00;
            printf("Are you sure you want to quit ? (o/N) : ");
            scanf("%c", &userInputChar);
            if(userInputChar == 'o') {
                QuitProgram();
            }
        } else if (userInput == '/') {
            system("cls");
            char userCommand[32] = {0};
            printf("/");
            scanf("%31s", userCommand);
            bool commandFound = false;
            for (int i = 0; commands[i].name != NULL; i++) {
                if (strcmp(userCommand, commands[i].name) == 0) {
                    commands[i].function();
                    commandFound = true;
                    break;
                }
            }
            if (!commandFound) {
                printf("command %s not recognised\n", userCommand);
            }
            printf("\nPress any key to continue");
            getch();
        }
    }
}