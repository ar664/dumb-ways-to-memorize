#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <SDL.h>
#include "globals.h"

#define GAME_NAME "Dumb Ways to Memorize"
#define SCREEN_RES_W 1280
#define SCREEN_RES_H 720
#define MAX_SPRITES 50
#define MAX_ANIMATIONS 20

typedef struct 
{
	vec2_t Position;
}Frame;

typedef struct sprite_s sprite_t;

struct sprite_s
{
	Frame mAnimations[20];
	Frame *mCurrentFrame;
	SDL_Texture *mTexture;
	vec2_t mSize;

};

void InitGraphics();
Frame *LoadAnimation(vec2_t *positions);
sprite_t *LoadSprite(const char *name, int flags);
sprite_t *LoadSpriteWithAnimation();
int DrawSprite(sprite_t *sprite, vec2_t *position, SDL_Renderer *renderer);

extern sprite_t *gSprites;

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;

#endif