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
# include <time.h>

//includes de librairies externes
# include "SDL.h"
# include "SDL_ttf.h"
# include "cJSON.h"

//includes persos, liens avec d'autres fichiers
# include "keys_mapping_json.h"
# include "remapping.h"
# define game_board_size 17

typedef struct {
    char *name;
    char *desc;
    void (*function)(void);
} Command;
typedef struct {
    int x,y;
} SnakeSegment;

bool snake_mod = false;
int foodX = 0, foodY = 0;
bool game_board[game_board_size][game_board_size];
int coordX = (int)(game_board_size / 2);
int coordY = (int)(game_board_size / 2);
int userInput = 0;
SnakeSegment snake[(game_board_size - 2)*(game_board_size - 2)];
int snakeLength = 1;

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

void generate_food(void) {
    bool isInSnake = false;
    do {
        isInSnake = false;
        foodX = rand() % (game_board_size - 2) + 1;
        foodY = rand() % (game_board_size - 2) + 1;
        for (int i = 0; i < snakeLength; i++)
        {
            if (snake[i].x == foodX && snake[i].y == foodY) {
                isInSnake = true;
            }
        }
        
    } while (game_board[foodY][foodX] || isInSnake);
}

//logique pour les commandes

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

void snake_mod_command(void) {
    snake_mod = true;
    generate_food();
    printf("Successfully transformed program in snake game\n");
}

void normal_mod_command(void) {
    snake_mod = false;
    printf("Successfully transformed program in normal mode\n");
}

Command commands[] = {
    {"help", "shows the available commands", help},
    {"config", "opens a window to change keybinding", config},
    {"snakemod", "makes the program a literal sneik game", snake_mod_command},
    {"normalmod", "makes the program go back to normal", normal_mod_command},
    {NULL, NULL, NULL}
};

void render_snake_game(void) {
    system("cls");
    for (int i = 0; i < game_board_size; i++)
    {
        for (int j = 0; j < game_board_size; j++)
        {
            if (i == coordY && j == coordX) {
                printf("*");
            }else if (i == foodY && j == foodX)
            {
                printf("@");
            } else if (game_board[i][j])
            {
                printf("#");
            } else {
                bool isBody = false;
                for (int k = 0; k < snakeLength; k++)
                {
                    if (snake[k].x == j && snake[k].y == i) {
                        printf("O");
                        isBody = true;
                        break;
                    }
                }
                if (!isBody) {
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

void update_snake(void) {
    for (int i = snakeLength - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
    snake[0].x = coordX;
    snake[0].y = coordY;
}

void render_normal_game(void) {
    for (int i = 0; i < game_board_size; i++)
    {
        for (int j = 0; j < game_board_size; j++)
        {
            if (i == coordY && j == coordX) {
                printf("*");
            } else  if (game_board[i][j]) {
                printf("#");
            }
            else {
            printf(" ");
            }
        }
        printf("\n");
    }
}

void game_over(void) {
    while (true)
    {
        printf("Game Over!\n1. Restart Snake\n2. Switch to Normal Mode\n3. Quit\n");
        char choice = getch();
        if (choice == '1') {
            coordX = (int)(game_board_size / 2);
            coordY = (int)(game_board_size / 2);
            snakeLength = 1;
            break;
        } else if (choice == '2') {
            normal_mod_command();
            coordX = (int)(game_board_size / 2);
            coordY = (int)(game_board_size / 2);
            break;
        } else if (choice == '3') {
            QuitProgram();
        } else {
            printf("Invalid choice. Please choose between 1-3\n");
        }
    }
}


int main(void) {
    srand(time(NULL));
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
    for (int i = 0; i < game_board_size; i++)
    {
        for (int j = 0; j < game_board_size; j++)
        {
            if (i == 0 || i == game_board_size - 1 || j == 0 || j == game_board_size - 1) {
                game_board[i][j] = 1;
                continue;
            }
            game_board[i][j] = 0;
        }
        
    }

    while (true)
    {
        system("cls");

        printf("Mode actuel : %s\n", snake_mod ? "Snake" : "Normal");
        snake_mod ? render_snake_game() : render_normal_game();
        userInput = getch();
        if (userInput == 224) {
            userInput = getch();
        }
        if (userInput == game_keys[0].key) {
            coordY--;
        } else if (userInput == game_keys[1].key) {
            coordY++;
        } else if (userInput == game_keys[2].key) {
            coordX--;
        } else if (userInput == game_keys[3].key) {
            coordX++;
        } else if (userInput == game_keys[4].key) {
            char userInputChar = 0x00;
            printf("Are you sure you want to quit ? (o/N) : ");
            fflush(stdin);
            scanf("%c", &userInputChar);
            if(userInputChar == 'o') {
                QuitProgram();
            }
            continue;
        } else if (userInput == '/') {
            system("cls");
            char userCommand[32] = {0};
            printf("/");
            fflush(stdin);
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
            continue;
        }
        if (snake_mod) {
            if (coordX == foodX && coordY == foodY) {
                snakeLength++;
                generate_food();
            }

            for (int i = 0; i < snakeLength - 1; i++)
            {
                if (coordX == snake[i].x && coordY == snake[i].y) {
                    game_over();
                }
            }

            update_snake();
            
            if (coordY <= 0 || coordY >= game_board_size - 1 || coordX <= 0 || coordX >= game_board_size - 1) {
                game_over();
            }
        } else {
            if (coordY <= 0) {
                coordY = game_board_size - 2;
            } else if (coordY >= game_board_size - 1) {
                coordY = 1;
            } else if (coordX <= 0) {
                coordX = game_board_size - 2;
            } else if (coordX >= game_board_size - 1) {
                coordX = 1;
            }
        }
        
    }
}