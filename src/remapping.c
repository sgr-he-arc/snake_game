# include "SDL.h"
# include "SDL_ttf.h"
# include <stdio.h>
# include "keys_mapping_json.h"

void config_mapping_view(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Initialiser SDL_ttf
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("config", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();  // Quitter proprement si la fenêtre n'est pas créée
        // QuitError();
        exit(1);
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        // QuitError();
        exit(1);
    }

    int quit = 0;
    SDL_Event e;
    while (!quit) {
        // Gérer les événements (fermeture de la fenêtre)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        // Creating a white background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyWindow(window);
}