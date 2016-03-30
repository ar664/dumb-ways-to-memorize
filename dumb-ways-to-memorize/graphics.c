#include "globals.h"
#include "graphics.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Surface *gRedSurface = NULL;
SDL_Texture *gRedTexture = NULL;
SDL_Renderer *gRedRenderer = NULL;
sprite_t *gSprites = NULL;
int gLastSprite = 0;
int gScreenWidth = 0;
int gScreenHeight = 0;

int InitGraphics()
{
	Uint32 flags = 0;
	SDL_DisplayMode current;
	if(SDL_Init(SDL_INIT_EVERYTHING))
	{
		printf("SDL can't initialize : %s", SDL_GetError());
		exit(-1);
	}
	if(SDL_GetDisplayMode(0,0,&current))
	{
		printf("No video mode: %s", SDL_GetError());
		exit(-1);
	}

	//Create Window based on either default or set screenWidth / Height
	if(gScreenHeight && gScreenWidth)
	{
		if( (gWindow = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, flags )) == NULL )
		{
			printf("Can't create window %s", SDL_GetError());
			exit(-1);
		}
	} else {
		if( (gWindow = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_RES_W, SCREEN_RES_H, flags )) == NULL )
		{
			printf("Can't create window %s", SDL_GetError());
			exit(-1);
		}
		gScreenHeight = SCREEN_RES_H;
		gScreenWidth = SCREEN_RES_W;
	}

	//Main Rendering Context
	SDL_SetWindowTitle(gWindow, "Dumb Ways to Memorize");

	if( (gRenderer = SDL_CreateRenderer(gWindow, -1, flags)) == NULL)
	{
		printf("Can't create renderer %s ", SDL_GetError());
		exit(-1);
	}

	//Red Renderer
	if ( (gRedSurface = SDL_CreateRGBSurface(0,gScreenWidth,gScreenHeight,32,0,0,0,0)) == NULL)
	{
		printf("Can't create red surface : %s \n", SDL_GetError());
	}
	if( (gRedRenderer = SDL_CreateSoftwareRenderer(gRedSurface)) == NULL)
	{
		printf("Can't create red renderer : %s \n", SDL_GetError());
	}
	if( SDL_SetRenderDrawColor(gRedRenderer, 0xFF, 0, 0, 0xFF) )
	{
		printf("Can't set red renderer : %s \n", SDL_GetError());
	}
	SDL_RenderClear(gRedRenderer);
	if( (gRedTexture = SDL_CreateTextureFromSurface(gRenderer, gRedSurface)) == NULL)
	{
		printf("Can't create red texture : %s \n", SDL_GetError());
	}
	gSprites = (sprite_t*) malloc(sizeof(sprite_t)*MAX_SPRITES);
	if(!gSprites)
	{
		printf("Could not allocate sprites");
		return -1;
	}
	memset(gSprites, 0, sizeof(sprite_t)*MAX_SPRITES);
	gLastSprite = 0;
	atexit(SDL_Quit);
	atexit(ShutdownGraphics);
	return 0;
}

void ShutdownGraphics()
{
	int i;
	for(i = 0; i < MAX_SPRITES; i++)
	{
		if(!gSprites[i].refCount)
		{
			continue;
		}
		FreeSprite(&gSprites[i]);

	}
	free(gSprites);
}

sprite_t *LoadSprite(const char *name, int flags)
{
	SDL_Surface *temp;
	sprite_t *check;
	int position;
	if(!gSprites)
	{
		return NULL;
	}
	if(abs(gLastSprite) > MAX_SPRITES)
	{
		gLastSprite = 0;
	}
	if(!name)
	{
		printf("No gPlayerName given to load sprite");
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
	if(!temp)
	{
		printf("Could not load image : %s \n", name);
		return NULL;
	}
	check->mSize.x = temp->w;
	check->mSize.y = temp->h;
	check->mTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
	check->name = strdup(name);
	check->refCount = 1;
	check->mFrames = 0;
	SDL_FreeSurface(temp);
	gLastSprite = position;
	return check;
}

int DrawSprite(sprite_t *sprite, int* frame, vec2_t * position, SDL_Renderer * renderer)
{
	SDL_Rect src, dst;
	int zero = 0;
	if(!sprite)
	{
		printf("Null sprite given \n");
		return -1;
	}
	if(!frame)
	{
		frame = &zero;
	}
	if( *frame >= sprite->mFrames )
	{
		*frame = 0;
	}
	
	SDL_SetRect(&src, sprite->mAnimations[*frame].Position.x, sprite->mAnimations[*frame].Position.y, sprite->mSize.x, sprite->mSize.y);
	
	*frame = *frame+1;

	if(!position)
	{
		SDL_SetRect(&dst, 0, 0, 0, 0);
		SDL_GetRendererOutputSize(renderer, &dst.w, &dst.h);
	} else {
		SDL_SetRect(&dst, position->x, position->y, sprite->mSize.x, sprite->mSize.y);
	}


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
	cols = frame_width ? width/frame_width : 1;
	rows = frame_height ? height/frame_height : 1;
	retVal = (Frame*) malloc(sizeof(Frame)*(rows*cols+1));
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols; j++)
		{
			retVal[i*rows + j].Position.x = j*frame_width;
			retVal[i*rows + j].Position.y = i*frame_height;
		}
	}
	memset(&retVal[rows*cols+1], 0, sizeof(Frame));
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

//Unused for now
void IncrementFrame(sprite_t* sprite)
{
	int frames;
	if(!sprite)
	{
		return;
	}
	frames = CountMem(&sprite->mAnimations[0], sizeof(Frame));
	if(!frames)
	{
		return;
	}
}

void SDL_SetRect(SDL_Rect* rect, int x, int y, int w, int h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}
