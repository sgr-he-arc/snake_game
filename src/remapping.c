# include "SDL.h"
# include "SDL_ttf.h"
# include <stdio.h>
# include <stdbool.h>

# include "remapping.h"
# include "keys_mapping_json.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 50;
const int BUTTON_MARGIN = 20;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

void destroy_tools(void) {
    if (font) {
        TTF_CloseFont(font);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
}

void handle_button_click(SDL_Event* e) {
    int mouseX = e->button.x;
    int mouseY = e->button.y;

    int x = 150 + BUTTON_WIDTH;
    int y = 50;

    for (int i = 0; i < num_commands; i++) {
        SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
        if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
        mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
            printf("clicked");
            return;
        }
        y += BUTTON_HEIGHT + BUTTON_MARGIN;
    }
}

void draw_button(int x, int y, int key) {
    SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &buttonRect);

    char keyText[32];
    snprintf(keyText, sizeof(keyText), "%c", key);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, keyText, (SDL_Color){0, 0, 0, 255});
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {
        x + (BUTTON_WIDTH - textSurface->w) / 2,
        y + (BUTTON_HEIGHT - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void draw_remapping_interface(void) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {0, 0, 0, 255};

    int x = 50;
    int y = 50;

    for (int i = 0; i < num_commands; i++) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, game_keys[i].label, textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        draw_button(x + BUTTON_WIDTH + 100, y, game_keys[i].key);

        y += BUTTON_HEIGHT + BUTTON_MARGIN;
    }

    SDL_RenderPresent(renderer);
}

int config_mapping_view(void) {
    window = SDL_CreateWindow("config", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        destroy_tools();
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        destroy_tools();
        return 1;
    }

    font = TTF_OpenFont("../assets/fonts/PressStart2P-Regular.ttf", 24);
    if (!font) {
        printf("Error occured while loading font: %s\n", TTF_GetError());
        destroy_tools();
        return 1;
    }
    int quit = 0;
    SDL_Event e;
    bool redraw = true;
    while (!quit) {
        // Gérer les événements (fermeture de la fenêtre)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                handle_button_click(&e);
                redraw = true;
            }
        }
        if(redraw) {
            draw_remapping_interface();
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
            redraw = false;
        }
    }
    destroy_tools();
    return 0;
}