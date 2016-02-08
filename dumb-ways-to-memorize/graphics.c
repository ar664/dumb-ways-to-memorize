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
	gSprites[size].mSize.x = temp->w;
	gSprites[size].mSize.y = temp->h;
	gSprites[size].mTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
	free(temp);
	memset(&gSprites[size+1], 0, sizeof(sprite_t));

	return &gSprites[size];
}

int DrawSprite(sprite_t *sprite, vec2_t *position, SDL_Renderer *renderer)
{
	return NULL;
}

Frame *LoadAnimation(int frame_width, int frame_height, int width, int height)
{
	Frame *retVal;
	int i,j, rows, cols;
	cols = width/frame_width;
	rows = height/frame_height;
	retVal = (Frame*) calloc(1, sizeof(Frame));
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			retVal = (Frame*) realloc(retVal, sizeof(i+1));
			retVal[i*rows + j].Position.x = j*frame_width;
			retVal[i*rows + j].Position.y = i*frame_height;
		}
	}
	return retVal;
}


