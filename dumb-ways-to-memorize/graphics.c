#include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
sprite_t *gSprites;
int gLastSprite = 0;

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

	gSprites = (sprite_t*) malloc(sizeof(sprite_t)*MAX_SPRITES);
	memset(gSprites, 0, sizeof(sprite_t)*MAX_SPRITES);
}



sprite_t *LoadSprite(const char *name, int flags)
{
	SDL_Surface *temp;
	sprite_t *check;
	int position;
	if(gLastSprite > MAX_SPRITES)
	{
		return NULL;
	}
	if( (check = FindSprite(name, NULL)) != NULL)
	{
		check->refCount++;
		return check;
	}
	if( (check = FindFreeSprite(&position)) == NULL)
	{
		printf("No free sprites");
		return NULL;
	}
	temp = IMG_Load(name);
	check->mSize.x = temp->w;
	check->mSize.y = temp->h;
	check->mTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
	check->name = strdup(name);
	check->refCount = 1;
	SDL_FreeSurface(temp);
	gLastSprite = position;
	return check;
}

int DrawSprite(sprite_t *sprite, vec2_t *position, SDL_Renderer *renderer)
{
	SDL_Rect src, dst;
	SDL_SetRect(&src, sprite->mCurrentFrame->Position.x, sprite->mCurrentFrame->Position.y, sprite->mSize.x, sprite->mSize.y);
	SDL_SetRect(&dst, position->x, position->y, sprite->mSize.x, sprite->mSize.y);


	if( SDL_RenderCopy(renderer, sprite->mTexture, &src, &dst) )
	{
		printf("Could not draw sprite %s : %s", sprite->name, SDL_GetError());
	}
	return 0;
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

sprite_t* FindSprite(const char* name, int* position)
{
	int i;
	for(i = 0; i < MAX_SPRITES; i++)
	{
		if(gSprites[i].refCount == 0)
			continue;
		if(!strcmp(gSprites[i].name, name))
		{
			if(position)
				*position = i;
			return &gSprites[i];
		}
	}
	return NULL;
}

sprite_t* FindFreeSprite(int *position)
{
	int i;
	for(i = gLastSprite; i < MAX_SPRITES; i++)
	{
		if(gSprites[i].refCount == 0)
		{
			return &gSprites[i];
		}
	}
	for(i = 0; i < gLastSprite; i++)
	{
		if(gSprites[i].refCount == 0)
		{
			return &gSprites[i];
		}
	}
	return NULL;
}

void FreeSprite(sprite_t *sprite)
{
	int position;
	if(sprite == FindSprite(sprite->name, &position))
	{
		sprite->refCount--;
		if(sprite->refCount > 0)
			return;
	}
	SDL_DestroyTexture(sprite->mTexture);
	free(sprite->name);
	memset(&gSprites[position], 0, sizeof(sprite_t) );

}

void SDL_SetRect(SDL_Rect* rect, int x, int y, int w, int h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}
