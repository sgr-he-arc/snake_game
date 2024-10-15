# nom de l'exe
EXEC = ./bin/chap04_ex4_snake.exe

# nom de l'exe (windows mode)
EXEC_WIN = .\bin\chap04_ex4_snake.exe

# variables pour les dossiers
PKG = ./packages
FNT = ./assets/fonts
SRC = ./src
OBJ = ./bin/objects
INC = ./include

# variables pour les dossiers (windows mode)
PKG_WIN = .\packages
FNT_WIN = .\assets\fonts
SRC_WIN = .\src
OBJ_WIN = .\bin\objects
INC_WIN = .\include

# lignes pour trouver les addons du projet
INCLUDES = -I$(PKG)/SDL2/include/SDL2 -I$(PKG)/SDL2_ttf/include/SDL2 -I$(PKG)/cJSON/include -I$(INC)
LIBS = -L$(PKG)/SDL2/lib -L$(PKG)/SDL2_ttf/lib -lSDL2 -lSDL2_ttf -lSDL2main



# variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -g -mconsole $(INCLUDES)

# Liste des fichiers sources et objets
SRC_FILES = $(wildcard $(SRC)/*.c) $(PKG)/cJSON/src/cJSON.c
OBJ_FILES = $(SRC_FILES:$(SRC)/%.c=$(OBJ)/%.o)

# Règle par défaut
all: $(EXEC)

# Travail de compilation de l'exécutable
$(EXEC): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ_FILES) $(LIBS)

# Travail de compilation des fichiers objets
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	if exist $(OBJ_WIN)\*.o del /Q $(OBJ_WIN)\*.o
	if exist $(EXEC_WIN) del /Q $(EXEC_WIN)

clean_objects:
	if exist $(OBJ_WIN)\*.o del /Q $(OBJ_WIN)\*.o