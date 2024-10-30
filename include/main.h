#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

//includes persos, liens avec d'autres fichiers
#include "keys_mapping_json.h"
#include "remapping.h"

#define game_board_size 22

typedef struct {
    char *name;
    char *desc;
    void (*function)(void);
} Command;

typedef struct {
    int x, y;
} SnakeSegment;

void quitProgram(void);
void quitError(void);
void generate_food(void);
void help(void);
void config(void);
void snake_mod_command(void);
void normal_mod_command(void);
void render_snake_game(void);
void render_normal_game(void);
void update_snake(void);
void game_over(void);
void checkCollisions(void);

#endif // MAIN_H
