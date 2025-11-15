#include <SDL2/SDL_ttf.h>
#include "stdio.h"
#include <stdlib.h> // <-- ADDED: For exit

#include "structs.h" // We need this for app.renderer
#include "text.h"

static TTF_Font *font_normal;
static TTF_Font *font_title;
static TTF_Font *font_italic;

extern App app; // Get the global app variable

void initFont(void)
{
    // Initialize the TTF library
    if (TTF_Init() == -1)
    {
        printf("Failed to init TTF: %s\n", TTF_GetError());
        exit(1);
    }
    
    font_normal = TTF_OpenFont("OpenSans-Regular.ttf", 28); 
    if (font_normal == NULL)
    {
        printf("Failed to load font (normal): %s\n", TTF_GetError());
        exit(1);
    }
   
    font_title = TTF_OpenFont("OpenSans-Regular.ttf", 48); 
    if (font_title == NULL)
    {
        printf("Failed to load font (title): %s\n", TTF_GetError());
        exit(1);
    }

    font_italic = TTF_OpenFont("OpenSans-Italic.ttf", 34); 
    if (font_normal == NULL)
    {
        printf("Failed to load font (italic): %s\n", TTF_GetError());
        exit(1);
    }
}

void drawText(int x, int y, int r, int g, int b, FontChoice fontToUse, const char *text)
{
    // --- FIXED: Cast ints to Uint8 to remove warnings ---
    SDL_Color color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
    
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect dest;
    
    // Create a surface from the text
    TTF_Font *selectedFont = NULL;
    switch (fontToUse)
    {
        case FONT_NORMAL:
            selectedFont = font_normal;
            break;
        case FONT_TITLE:
            selectedFont = font_title;
            break;
        case FONT_ITALIC:
            selectedFont = font_italic;
            break;
    }
    surface = TTF_RenderText_Solid(selectedFont, text, color);
    if (surface == NULL)
    {
        printf("Failed to create text surface: %s\n", TTF_GetError());
        return;
    }
    
    // Create a texture from the surface
    texture = SDL_CreateTextureFromSurface(app.renderer, surface);
    if (texture == NULL)
    {
        printf("Failed to create text texture: %s\n", TTF_GetError());
        SDL_FreeSurface(surface);
        return;
    }
    
    // Set the destination rectangle
    dest.x = x;
    dest.y = y;
    dest.w = surface->w;
    dest.h = surface->h;
    
    // Draw the texture and clean up
    SDL_RenderCopy(app.renderer, texture, NULL, &dest);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void cleanupFont(void)
{
    // Free the font
    if (font_normal != NULL)
    {
        TTF_CloseFont(font_normal);
    }
    if (font_italic != NULL)
    {
        TTF_CloseFont(font_italic);
    }
    if (font_title != NULL)
    {
        TTF_CloseFont(font_title);
    }
    
    // Quit the TTF library
    TTF_Quit();
}