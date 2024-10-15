#define SDL_MAIN_HANDLED
# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <conio.h>
# include <signal.h>
# include <windows.h>
# include <string.h>

# include "cJSON.h"

int upKey = 0;
int downKey = 0;
int leftKey = 0;
int rightKey = 0;
int quitKey = 0;

void QuitProgram(void) {
    SDL_Quit();
    TTF_Quit();
    exit(0);
}

void QuitError(void) {
    SDL_Quit();
    TTF_Quit();
    exit(-1);
}

int map_key(const char* key) {
    if (strcmp(key, "ArrowUp") == 0) {
        return 72;  // Code pour la flèche haut
    } else if (strcmp(key, "ArrowDown") == 0) {
        return 80;  // Code pour la flèche bas
    } else if (strcmp(key, "ArrowLeft") == 0) {
        return 75;  // Code pour la flèche gauche
    } else if (strcmp(key, "ArrowRight") == 0) {
        return 77;  // Code pour la flèche droite
    } else if (strlen(key) == 1) {
        return key[0];  // Retourner un caractère normal
    }
    else {
        QuitError();
        return -1;
    }
}

int read_config_file(void) {
    FILE* file = fopen("../assets/config/settings.json", "rb"); // lit le fichier en binaire
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier settings.json\n");
        return 1;
    }

    // Trouver la taille du fichier
    fseek(file, 0, SEEK_END); // place le curseur à la fin du fichier
    long length = ftell(file); // récupère la taille du fichier du début jusqu'au curseur
    fseek(file, 0, SEEK_SET); // place le curseur au début du fichier

    // mets le fichier dans la mémoire avec un tampon (liste de C)
    char* data = (char*)malloc(length + 1);
    if (data) {
        fread(data, 1, length, file); // lit length fois 1 char et le met dans data
    }
    fclose(file); // ferme le fichier
    data[length] = '\0'; // pour dire que c'est la fin de la liste
    
    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        printf("Erreur lors du parse du json\n");
        free(data);
        return 1;
    }

    cJSON *controls = cJSON_GetObjectItemCaseSensitive(json, "controls");
    if (controls) {
        cJSON *up = cJSON_GetObjectItemCaseSensitive(controls, "move_up");
        cJSON *down = cJSON_GetObjectItemCaseSensitive(controls, "move_down");
        cJSON *left = cJSON_GetObjectItemCaseSensitive(controls, "move_left");
        cJSON *right = cJSON_GetObjectItemCaseSensitive(controls, "move_right");
        cJSON *quit = cJSON_GetObjectItemCaseSensitive(controls, "quit");
        // Lire les valeurs des touches et les afficher
        if (cJSON_IsString(up) && (up->valuestring != NULL)) {
            upKey = map_key(up->valuestring);
        }
        else {
            return 1;
        }
        if (cJSON_IsString(down) && (down->valuestring != NULL)) {
            downKey = map_key(down->valuestring);
        }
        else {
            return 1;
        }
        if (cJSON_IsString(left) && (left->valuestring != NULL)) {
            leftKey = map_key(left->valuestring);
        }
        else {
            return 1;
        }
        if (cJSON_IsString(right) && (right->valuestring != NULL)) {
            rightKey = map_key(right->valuestring);
        }
        else {
            return 1;
        }
        if (cJSON_IsString(quit) && (quit->valuestring != NULL)) {
            quitKey = map_key(quit->valuestring);
        }
        else {
            return 1;
        }
    }
    else {
        return 1;
    }
    return 0;
}



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

}

Command commands[] = {
    {"help", "shows the available commands", help},
    {"config", "opens a window to change keybinding", config},
    {NULL, NULL, NULL}
};

int main(void) {
    if (read_config_file() == 1) {
        printf("An error occured, please check the config file");
        getch();
        QuitError();
    }

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
        if (userInput == upKey) {
            coordY--;
            if (coordY < 0) {
                coordY += game_board_size;
            }
        } else if (userInput == downKey) {
            coordY++;
            if (coordY > game_board_size - 1) {
                coordY -= game_board_size;
            }
        } else if (userInput == leftKey) {
            coordX--;
            if (coordX < 0) {
                coordX += game_board_size;
            }
        } else if (userInput == rightKey) {
            coordX++;
            if (coordX > game_board_size - 1) {
                coordX -= game_board_size;
            }
        } else if (userInput == quitKey) {
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