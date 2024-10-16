#ifndef JSON_UTILS_H
#define JSON_UTILS_H

typedef struct
{
    char* label;
    int key;
} KeyMapping;

extern KeyMapping* game_keys;
extern int num_commands;

int read_config_file(void);

void free_commands(void);

#endif