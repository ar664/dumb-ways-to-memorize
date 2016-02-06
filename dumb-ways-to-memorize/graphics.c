#include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
sprite_t *gSprites;


void InitGraphics()
{
	Uint32 flags = 0;
	SDL_DisplayMode current;
	if(SDL_Init(SDL_INIT_EVERYTHING))
	{
		printf("SDL can't initialize : %s", SDL_GetError());
		exit(0);
	}
	if(SDL_GetDisplayMode(0,0,&current))
	{
		printf("No video mode: %s", SDL_GetError());
		exit(0);
	}

	if( (gWindow = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_RES_W, SCREEN_RES_H, flags )) == NULL )
	{
		printf("Can't create window %s", SDL_GetError());
	}

	if( (gRenderer = SDL_CreateRenderer(gWindow, -1, flags)) == NULL)
	{
		printf("Can't create window %s", SDL_GetError());
	}

	gSprites = (sprite_t*) calloc(1, sizeof(sprite_t));

}

sprite_t *LoadSprite(const char *name, int flags)
{
	SDL_Surface *temp;
	int size;
	size = CountMem(gSprites, sizeof(sprite_t));
	if(size > MAX_SPRITES)
	{
		return NULL;
	}
	temp = IMG_Load(name);
	gSprites = (sprite_t*) realloc(gSprites, sizeof(sprite_t)*(size+1));
	gSprites[size].mTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
	free(temp);
	memset(&gSprites[size+1], 0, sizeof(sprite_t));

	return &gSprites[size];
}

int DrawSprite(sprite_t *sprite, vec2_t *position, SDL_Renderer *renderer)
{
	return NULL;
}
