#include "structs.h"
#include <SDL2/SDL.h>
#include <input.h>

void doInput(void){
    SDL_Event event;

    // 1. Process one-time events (QUIT, button DOWN/UP presses)
    while( SDL_PollEvent(&event)){
        switch( event.type ){
            case SDL_QUIT:
                exit(0);
                break;
            // Remove the cases for SDL_KEYDOWN and SDL_KEYUP
            default: 
                break;
        }
    }    
    app.keyboard = SDL_GetKeyboardState(NULL);
}
