#include <SDL2/SDL.h>
#include "defs.h"

#ifndef STRUCTS_H
#define STRUCTS_H

typedef void (*FunctionPtr)(void);

typedef struct{
    FunctionPtr logic;
    FunctionPtr draw;
} Delegate;

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    Delegate delegate;
    const Uint8 *keyboard;
    
}App;

typedef struct Entity {
    float x;
    float y;
    int w;
    int h;
    float dx;
    float dy;
    int health;
    int side;
    int reload;//used for player's firing rate
    SDL_Texture *texture;
    struct Entity *next;
} Entity;

typedef struct Explosion{
    float x;
    float y;
    float dx;
    float dy;
    int r, g, b, a;
    struct Explosion *next;
} Explosion;

typedef struct Debris
{
    float x;
    float y;
    float dx;
    float dy;
    SDL_Rect rect;
    SDL_Texture *texture;
    int life;
    struct Debris *next;    
} Debris;

typedef struct{
    Entity fighterHead;
    Entity *fighterTail;
    Entity bulletHead;
    Entity *bulletTail;
    Explosion explosionHead, *explosionTail;
    Debris debrisHead, *debrisTail; 
} Stage;

typedef struct{
    int x;
    int y;
    int speed;
} Star;

typedef enum{
    STATE_TITLE_SCREEN,
    STATE_GAME,
    STATE_GAME_OVER,
} GameState;

extern App app; 
extern Entity *player;
extern Entity *enemy;
extern Stage stage;
extern GameState currentGameState;
extern int score;
extern int highScore;

typedef void (*FunctionPtr)(void);

#endif // STRUCTS_H