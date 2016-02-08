#include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
sprite_t *gSprites;
int gSpriteNum = 0;

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
	sprite_t *check;
	if(gSpriteNum > MAX_SPRITES)
	{
		return NULL;
	}
	if( (check = FindSprite(name)) != NULL)
	{
		check->refCount++;
		return check;
	}
	temp = IMG_Load(name);
	gSprites = (sprite_t*) realloc(gSprites, sizeof(sprite_t)*(gSpriteNum+1));
	gSprites[gSpriteNum].mSize.x = temp->w;
	gSprites[gSpriteNum].mSize.y = temp->h;
	gSprites[gSpriteNum].mTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
	gSprites[gSpriteNum].name = strdup(name);
	gSprites[gSpriteNum].refCount = 1;
	free(temp);
	memset(&gSprites[gSpriteNum+1], 0, sizeof(sprite_t));
	gSpriteNum++;
	return &gSprites[gSpriteNum];
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

sprite_t* FindSprite(const char* name)
{
	int i;
	for(i = 0; i < gSpriteNum; i++)
	{
		if(!strcmp(gSprites[i].name, name))
		{
			return &gSprites[i];
		}
	}
	return NULL;
}

void FreeSprite(sprite_t *sprite)
{
	int position, i;
	if(sprite == FindSprite(sprite->name))
	{
		sprite->refCount--;
		if(sprite->refCount > 0)
			return;
	}
	position = -1;
	for(i = 0; i < gSpriteNum; i++)
	{
		if(&gSprites[i] == sprite)
		{
			position = i;
			break;
		}
	}
	if(position == -1)
		return;
	SDL_DestroyTexture(sprite->mTexture);
	memmove(&gSprites[position], &gSprites[position+1], sizeof(sprite_t)*abs(position-gSpriteNum));
	gSpriteNum--;

}

void SDL_SetRect(SDL_Rect* rect, int x, int y, int w, int h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}
