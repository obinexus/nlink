// my-nlinked-project-for-sdl/game-windows/include/game_window.h
#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <SDL2/SDL.h>

// Schema-less component (more flexible but requires consciousness preservation)
typedef struct game_window game_window_t;

// Function declarations
game_window_t* game_window_create(const char* title, int x, int y, int width, int height);
void game_window_destroy(game_window_t* window);
SDL_Window* game_window_get_sdl_window(game_window_t* window);
SDL_Renderer* game_window_get_renderer(game_window_t* window);
int game_window_process_events(game_window_t* window);
void game_window_render(game_window_t* window);

#endif // GAME_WINDOW_H
