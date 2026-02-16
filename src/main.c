#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h> // For memset
#include <stdlib.h> // For atexit
#include <stdio.h>
#include "structs.h"
#include "init.h"
#include "draw.h"
#include "input.h"
#include "defs.h"
#include "stage.h"
#include "util.h" 
#include "text.h"

App app;
Entity *player;
Stage stage;

GameState currentGameState;
int score;
int highScore;

SDL_Texture *playerTexture;
SDL_Texture *bulletTexture;
SDL_Texture *enemyTexture;
SDL_Texture *enemyBulletTexture;
SDL_Texture *background;
SDL_Texture *explosionTexture;

//  Background Scrolling Global 
int backgroundX = 0;

// Starfield Global 
Star stars[MAX_STARS];

//Function prototypes for our new states
void titleScreenLogic(void);
void titleScreenDraw(void);

/**
 * @brief Corrected cleanup function.
 * Frees all game entities and destroys all loaded textures.
 */
void cleanup(void)
{
    // 1. Free all entities (player, enemies, bullets, etc.)
    
    resetStage();

    // 2. Destroy all global textures that were loaded in initStage()
    if (playerTexture) SDL_DestroyTexture(playerTexture);
    if (bulletTexture) SDL_DestroyTexture(bulletTexture);
    if (enemyTexture) SDL_DestroyTexture(enemyTexture);
    if (enemyBulletTexture) SDL_DestroyTexture(enemyBulletTexture);
    if (background) SDL_DestroyTexture(background);
    if (explosionTexture) SDL_DestroyTexture(explosionTexture);

    cleanupFont();
    // 3. Shut down SDL subsystems
    IMG_Quit();
    
    if (app.renderer) {
        SDL_DestroyRenderer(app.renderer);
    }
    
    if (app.window) {
        SDL_DestroyWindow(app.window);
    }
    
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    long then;
    float remainder;

    memset(&app, 0, sizeof(app));

    //set the initial game state
    currentGameState = STATE_TITLE_SCREEN;
    score = 0;

    //loading highscore
    FILE *file = fopen(HIGHSCORE_FILE, "r");
    if( file == NULL){
        highScore = 0;
        file = fopen(HIGHSCORE_FILE, "w");
        if( file != NULL){
            fprintf(file, "%d", highScore);
            fclose(file);
        }
    }else{
        fscanf(file, "%d", &highScore);
        fclose(file);
    }

    initSDL();

    initFont();
    
    atexit(cleanup);
    
    initStage();
    
    then = SDL_GetTicks();
    remainder = 0;

    while (1)
    {
        prepareScene();
        
        doInput();

        switch(currentGameState){
            case STATE_TITLE_SCREEN:
                app.delegate.logic = titleScreenLogic;
                app.delegate.draw = titleScreenDraw;
                break;
            
            case STATE_GAME:
                app.delegate.logic = logic;
                app.delegate.draw = draw;
                break;
        }
        
        app.delegate.logic();
        
        app.delegate.draw();
        
        presentScene();
        
        capFrameRate(&then, &remainder);
    }
    
    return 0;
}

void titleScreenLogic(void)
{
    //check if "space" key pressed
    if( app.keyboard[SDL_SCANCODE_SPACE])
    {
        //switch to the current game
        currentGameState = STATE_GAME;
        resetStage();
        score = 0;
    }
}

void titleScreenDraw(void)
{
    drawBackground();
    drawStarfield();

    drawText(440,290,255,255,255,FONT_TITLE," SHOOTER GAME");
    drawText(480,450,255,255,255,FONT_ITALIC," Press Space to Play!");

    char highScoreText[32];
    sprintf(highScoreText, " HighScore: %d", highScore);
    drawText(520,500,255,255,0,FONT_NORMAL,highScoreText);
}