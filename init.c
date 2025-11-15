#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "structs.h"
#include "defs.h"
#include "stdio.h"

void initSDL(void){
    int renderFlags , windowFlags;
    
    renderFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = 0;
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        printf("couldn;t initalize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    app.window = SDL_CreateWindow("Shooter 01", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

    if(! app.window){
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH,SCREEN_HEIGHT,SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    app.renderer = SDL_CreateRenderer( app.window, -1, renderFlags);

    if(!app.renderer){
        printf("Failed to crete renderer: %s\n", SDL_GetError());
        exit(1);
    }
    int imgFlags = IMG_INIT_PNG;

    if (!(IMG_Init(imgFlags) & imgFlags)) {
    // Print an error message if initialization failed
        SDL_Log("SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
    // You might want to exit the program here
        }
    
}