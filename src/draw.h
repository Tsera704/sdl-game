#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>

// Main draw loop functions
void prepareScene(void);
void draw(void);
void presentScene(void);

// Texture loading
SDL_Texture *loadTexture(const char *filename);

// Drawing helpers
void blit(SDL_Texture* texture, int x, int y);
void blitRect(SDL_Texture *texture, SDL_Rect *src, int x, int y);
void blit_scaled(SDL_Texture* texture, SDL_Rect *src, int x, int y, float scale_factor);

// Component drawing functions (called by draw())
void drawBackground(void);
void drawStarfield(void);
void drawDebris(void);
void drawFighters(void);
void drawBullets(void);
void drawExplosions(void);

#endif // DRAW_H