//Spécifie à SDL que le projet est en console sous main
# define SDL_MAIN_HANDLED

//header principal
# include "main.h"

//Includes systèmes
# include <stdio.h>
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

bool snake_mod = false;
int foodX = 0, foodY = 0;
bool game_board[game_board_size][game_board_size];
int coordX = (int)(game_board_size / 2);
int coordY = (int)(game_board_size / 2);
int userInput = 0;
SnakeSegment snake[(game_board_size - 2)*(game_board_size - 2)];
int snakeLength = 1;
int directionX = 1;
int directionY = 0;
char snakeHead = '>';
Command commands[] = {
    {"help", "shows the available commands", help},
    {"config", "opens a window to change keybinding", config},
    {"snakemod", "makes the program a literal sneik game", snake_mod_command},
    {"normalmod", "makes the program go back to normal", normal_mod_command},
    {NULL, NULL, NULL}
};

int main(void) {
    srand(time(NULL));
    //Initialisation des outils graphiques
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        quitError();
    }

    
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        quitError();
    }

    //récupération des touches
    if (read_config_file()) {
        printf("An error occured, please check the config file");
        getch();
        quitError();
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
        if (_kbhit() || !snake_mod) {
            userInput = getch();
            if (userInput == 224) {
                userInput = getch();
            }
            if (userInput == game_keys[0].key) { //Go up
                if (snake_mod) {
                    directionX = 0;
                    directionY = -1;
                } else {
                    coordY--;
                }
                snakeHead = '^';
            } else if (userInput == game_keys[1].key) { //Go down
                if (snake_mod) {
                    directionX = 0;
                    directionY = 1;
                } else {
                    coordY++;
                }
                snakeHead = 'v';
            } else if (userInput == game_keys[2].key) { //Go left
                if (snake_mod) {
                    directionX = -1;
                    directionY = 0;
                } else {
                    coordX--;
                }
                snakeHead = '<';
            } else if (userInput == game_keys[3].key) { //Go right
                if (snake_mod) {
                    directionX = 1;
                    directionY = 0;
                } else {
                    coordX++;
                }
                snakeHead = '>';
            } else if (userInput == game_keys[4].key) { //quit
                char userInputChar = 0x00;
                printf("Are you sure you want to quit ? (o/N) : ");
                fflush(stdin);
                scanf("%c", &userInputChar);
                if(userInputChar == 'o') {
                    quitProgram();
                }
                continue;
            } else if (userInput == game_keys[5].key) { //pause (Jojo)
                printf("Game paused, press %c again to unpause...", game_keys[5].key);
                while (true) {
                    userInput = getch();
                    if (userInput == game_keys[5].key) {
                        break;
                    }
                }
            } else if (userInput == '/') { //command handle
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
            if (!snake_mod) {
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
        if (snake_mod) {
            Sleep(200);
            checkCollisions();
        }
    }
}

//méthodes pour quitter le programme
void quitProgram(void) {
    SDL_Quit();
    TTF_Quit();
    exit(0);
}
void quitError(void) {
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
        quitError();
    }
}

void snake_mod_command(void) {
    snake_mod = true;
    coordX = (int)(game_board_size / 2);
    coordY = (int)(game_board_size / 2);
    snakeLength = 1;
    snakeHead = '>';
    directionX = 1;
    directionY = 0;
    generate_food();
    printf("Successfully transformed program in snake game\n");
}

void normal_mod_command(void) {
    snake_mod = false;
    printf("Successfully transformed program in normal mode\n");
}

void render_snake_game(void) {
    system("cls");
    for (int i = 0; i < game_board_size; i++)
    {
        for (int j = 0; j < game_board_size; j++)
        {
            if (i == coordY && j == coordX) {
                printf("%c", snakeHead);
            } else if (i == foodY && j == foodX)
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
                printf("%c", snakeHead);
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
            quitProgram();
        } else {
            printf("Invalid choice. Please choose between 1-3\n");
        }
    }
}

void checkCollisions(void) {
    coordX += directionX;
    coordY += directionY;
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
}


//TODO: add async inputs to allow easier turns
//TODO: add graphic view for main game
//TODO: add unicode to the program
//TODO: test program for eventual bugs