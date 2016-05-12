#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <SDL.h>
#include "globals.h"

#define GAME_NAME "Dumb Ways to Memorize"
#define SCREEN_STRING "Screen Dimensions"
#define SCREEN_RES_W 1280
#define SCREEN_RES_H 720
#define MAX_SPRITES 500
#define MAX_ANIMATIONS 20

/** Defines Frame , which is an alias for Position of the start of the sprite */
typedef struct 
{
	vec2_t Position;
}Frame;

typedef struct sprite_s sprite_t;

/**
 * The structure of our sprites.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

struct sprite_s
{
	Frame mAnimations[MAX_ANIMATIONS];  /**< The animation positions of the given sprite*/
	SDL_Texture *mTexture;				/**< The texture of the sprite */
	vec2_t mSize;						/**< The size of the sprite frames */
	vec2_t mRawSize;					/**< The size of the image */
	int mFrames;						/**< The number of frames in this sprite */
	int mRefCount;						/**< Number of references, in the engine */
	int mMillisecondsPerFrame;				/**< The frames per second of this sprite */
	char *name;							/**< The name of the sprite's location */

};

/**
 * Init graphics system.
 * Calls SDL_Init and allocates memory for the sprites.
 *
 * @return	0 if good, -1 if failure.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */
int InitGraphics();

/**
 * Shutdown graphics system, which frees all sprite data.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void ShutdownGraphics();

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
Frame *LoadAnimation(int frame_width, int frame_height, int width, int height);

/**
 * Searches for the first sprite.
 *
 * @param	name				The name of the sprite file.
 * @param [in,out]	position	If non-null, the position.
 *
 * @return	null if it fails, else the found sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */
sprite_t *FindSprite(const char *name, int *position);

/**
 * Searches for the first free sprite in sprite system memory.
 *
 * @param [in,out]	position	If non-null, the position.
 *
 * @return	null if it fails, else the found free sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */
sprite_t *FindFreeSprite(int *position);

/**
 * Loads a sprite.
 *
 * @param	name 	The file name of the image to load.
 * @param	flags	The flags.
 *
 * @return	null if it fails, else the sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */
sprite_t *LoadSprite(const char *name, int flags);

/**
 * Draw the sprite. If mCurrentFrame not set, draws the first frame (0,0). 
 * If no position given, will draw as if was background.
 *
 * @param [in,out]	sprite  	If non-null, the sprite.
 * @param [in,out]	frame		If non-null, the frame number to draw.
 * @param [in,out]	position	If non-null, the position.
 * @param [in,out]	renderer	If non-null, the renderer.
 *
 * @return	0 if good, -1 if failure
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */
int DrawSprite(sprite_t *sprite, int frame, vec2_t * position, SDL_Renderer * renderer, int flags);

/**
 * Free the sprite memory, if recfcount - 1 > 0 nothing happens.
 *
 * @param [in,out]	sprite	If non-null, the sprite.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */
void FreeSprite(sprite_t *sprite);

void UpdateSprite(sprite_t *sprite);

/**
 * Sdl set rectangle dimensions.
 *
 * @param [in,out]	rect	If non-null, the rectangle to set.
 * @param	x				The x coordinate.
 * @param	y				The y coordinate.
 * @param	w				The width.
 * @param	h				The height.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void SDL_SetRect(SDL_Rect *rect, int x, int y, int w, int h);

extern sprite_t *gSprites;
extern int gLastSprite;
extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;
extern SDL_Surface *gRedSurface;
extern SDL_Texture *gRedTexture;
extern SDL_Renderer *gRedRenderer;

#endif