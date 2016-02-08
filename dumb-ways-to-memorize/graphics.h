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
	Frame mAnimations[MAX_ANIMATIONS];
	Frame *mCurrentFrame;
	SDL_Texture *mTexture;
	vec2_t mSize;
	int refCount;
	char *name;

};

void InitGraphics();
Frame *LoadAnimation(int frame_width, int frame_height, int width, int height);
sprite_t *FindSprite(const char *name);
sprite_t *LoadSprite(const char *name, int flags);
sprite_t *LoadSpriteWithAnimation();
int DrawSprite(sprite_t *sprite, vec2_t *position, SDL_Renderer *renderer);
void FreeSprite(sprite_t *sprite);
void SDL_SetRect(SDL_Rect *rect, int x, int y, int w, int h);

extern sprite_t *gSprites;
extern int gSpriteNum;
extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;

#endif