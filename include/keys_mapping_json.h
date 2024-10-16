#ifndef JSON_UTILS_H
#define JSON_UTILS_H

typedef struct
{
    char *label;
    int key;
} KeyMapping;

extern int upKey;
extern int downKey;
extern int leftKey;
extern int rightKey;
extern int quitKey;

int read_config_file(void);

void free_commands();

#endif