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
SDL_Renderer *gRedRenderer = NULL;
sprite_t *gSprites = NULL;
int gLastSprite = 0;
int gScreenWidth = 0;
int gScreenHeight = 0;
/**
 * Init graphics.
 *
 * @return	0 if good, -1 if failure.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

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

/**
 * Loads a sprite.
 *
 * @param	name 	The name.
 * @param	flags	The flags.
 *
 * @return	null if it fails, else the sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

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
		printf("No name given to load sprite");
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
	SDL_FreeSurface(temp);
	gLastSprite = position;
	return check;
}

/**
 * Draw the sprite. If mCurrentFrame not set, draws the first frame (0,0). 
 * If no position given, will draw as if was background.
 *
 * @param [in,out]	sprite  	If non-null, the sprite.
 * @param [in,out]	position	If non-null, the position.
 * @param [in,out]	renderer	If non-null, the renderer.
 *
 * @return	0 if good, -1 if failure
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

int DrawSprite(sprite_t *sprite, vec2_t *position, SDL_Renderer *renderer)
{
	SDL_Rect src, dst;
	if(!sprite)
	{
		printf("Null sprite given \n");
		return -1;
	}
	if(sprite->mCurrentFrame)
	{
		SDL_SetRect(&src, sprite->mCurrentFrame->Position.x, sprite->mCurrentFrame->Position.y, sprite->mSize.x, sprite->mSize.y);
	} else
	{
		SDL_SetRect(&src, 0, 0, sprite->mSize.x, sprite->mSize.y);
	}
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

/**
 * Loads an animation based on frame_width & frame_height.
 *
 * @param	frame_width 	Width of the frame.
 * @param	frame_height	Height of the frame.
 * @param	width			The width.
 * @param	height			The height.
 *
 * @return	null if it fails, else the animation.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

Frame *LoadAnimation(int frame_width, int frame_height, int width, int height)
{
	Frame *retVal;
	int i,j, rows, cols;
	cols = width/frame_width;
	rows = height/frame_height;
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

/**
 * Searches for the first sprite.
 *
 * @param	name				The name.
 * @param [in,out]	position	If non-null, the position.
 *
 * @return	null if it fails, else the found sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

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

/**
 * Searches for the first free sprite.
 *
 * @param [in,out]	position	If non-null, the position.
 *
 * @return	null if it fails, else the found free sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

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

/**
 * Free the sprite memory, if recfcount - 1 > 0 nothing happens.
 *
 * @param [in,out]	sprite	If non-null, the sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

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

void IncrementFrame(sprite_t* sprite)
{
	int frames;
	if(!sprite)
	{
		return;
	}
	frames = CountMem(sprite->mAnimations, sizeof(vec2_t));
	if(!frames)
	{
		return;
	}
	if(!sprite->mCurrentFrame || (sprite->mCurrentFrame - sprite->mAnimations >= frames) )
	{
		sprite->mCurrentFrame = sprite->mAnimations;
	} else
	{
		sprite->mCurrentFrame++;
	}
}

void SDL_SetRect(SDL_Rect* rect, int x, int y, int w, int h)
{
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}
