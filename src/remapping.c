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

int selected_button = -1;

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

void draw_button(int x, int y, int key, bool selected) {
    SDL_Rect buttonRect = {x, y, BUTTON_WIDTH, BUTTON_HEIGHT};
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 51, 51, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    }
    SDL_RenderFillRect(renderer, &buttonRect);

    char keyText[32];

    switch (key) {
        case 72:  // Flèche haut
            snprintf(keyText, sizeof(keyText), "Uarrop");
            break;
        case 80:  // Flèche bas
            snprintf(keyText, sizeof(keyText), "Darrow");
            break;
        case 75:  // Flèche gauche
            snprintf(keyText, sizeof(keyText), "Larrow");
            break;
        case 77:  // Flèche droite
            snprintf(keyText, sizeof(keyText), "Rarrow");
            break;
        default:
            // Si c'est une touche normale, on affiche simplement la lettre en majuscule
            snprintf(keyText, sizeof(keyText), "%c", key);
            break;
    }

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

void draw_remapping_interface(int selected) { // selected = 0 == aucun select
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

        if (selected == i + 1) {
            draw_button(x + BUTTON_WIDTH + 100, y, game_keys[i].key, true);
        }
        else {
            draw_button(x + BUTTON_WIDTH + 100, y, game_keys[i].key, false);
        }
        

        y += BUTTON_HEIGHT + BUTTON_MARGIN;
    }

    SDL_RenderPresent(renderer);
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
            selected_button = i;
            draw_remapping_interface(i + 1);
            SDL_StartTextInput();
            return;
        }
        y += BUTTON_HEIGHT + BUTTON_MARGIN;
    }
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
            } else if (e.type == SDL_TEXTINPUT && selected_button != -1) {
                game_keys[selected_button].key = e.text.text[0];
                SDL_StopTextInput();
                save_config_file();
                selected_button = -1;
                redraw = true;
            } else if (e.type == SDL_KEYDOWN && selected_button != -1) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        game_keys[selected_button].key = 72;
                        break;
                    case SDLK_DOWN:
                        game_keys[selected_button].key = 80;
                        break;
                    case SDLK_LEFT:
                        game_keys[selected_button].key = 75;
                        break;
                    case SDLK_RIGHT:
                        game_keys[selected_button].key = 77;
                        break;
                    default:
                        continue;
                }
                SDL_StopTextInput();
                save_config_file();
                selected_button = -1;
                redraw = true;
            }
        }
        if(redraw) {
            draw_remapping_interface(0);
            redraw = false;
        }
    }
    destroy_tools();
    return 0;
}