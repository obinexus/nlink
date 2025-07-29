// my-nlinked-project-for-sdl/game-windows/src/game_window.c
#include "game_window.h"
#include <stdlib.h>
#include <string.h>

// Schema-less implementation - structure only defined in the implementation file
struct game_window {
    // SDL components
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Component identification
    char* component_id;
    
    // Consciousness preservation
    struct {
        void* temporal_anchor;
        void* symbolic_residue;
        void* human_validation;
    } consciousness;
    
    // State
    int is_running;
};

// Create a game window
game_window_t* game_window_create(const char* title, int x, int y, int width, int height) {
    game_window_t* window = malloc(sizeof(game_window_t));
    if (!window) return NULL;
    
    // Initialize SDL window and renderer
    window->window = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_SHOWN);
    if (!window->window) {
        free(window);
        return NULL;
    }
    
    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);
    if (!window->renderer) {
        SDL_DestroyWindow(window->window);
        free(window);
        return NULL;
    }
    
    // Set component ID
    window->component_id = strdup("game-windows.core");
    
    // Set up consciousness anchors - will be populated by nlink during resolution
    window->consciousness.temporal_anchor = NULL;
    window->consciousness.symbolic_residue = NULL;
    window->consciousness.human_validation = NULL;
    
    // Set initial state
    window->is_running = 1;
    
    return window;
}

// Destroy a game window
void game_window_destroy(game_window_t* window) {
    if (!window) return;
    
    // Clean up SDL components
    if (window->renderer) {
        SDL_DestroyRenderer(window->renderer);
    }
    
    if (window->window) {
        SDL_DestroyWindow(window->window);
    }
    
    // Free component ID
    free(window->component_id);
    
    // Free window itself
    free(window);
}

// Get the SDL window
SDL_Window* game_window_get_sdl_window(game_window_t* window) {
    return window ? window->window : NULL;
}

// Get the SDL renderer
SDL_Renderer* game_window_get_renderer(game_window_t* window) {
    return window ? window->renderer : NULL;
}

// Process SDL events
int game_window_process_events(game_window_t* window) {
    if (!window) return 0;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            window->is_running = 0;
        }
    }
    
    return window->is_running;
}

// Render the window
void game_window_render(game_window_t* window) {
    if (!window || !window->renderer) return;
    
    // Clear renderer
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->renderer);
    
    // Render content here
    
    // Present renderer
    SDL_RenderPresent(window->renderer);
}
