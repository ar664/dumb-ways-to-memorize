#ifndef __MENU_H
#define __MENU_H

#include "globals.h"
#include "graphics.h"
#include "parseobject.h"

#define	MENU_ITEM_MAX	20
#define MAX_MENUS		10
#define MENU_BACKGROUND "Background"
#define MENU_ITEMS		"Items"
#define MENU_ITEM_SPRITE "sprite"
#define MENU_ITEM_TEXT	"text"
#define MENU_ITEM_LINK	"link"
#define MENU_TYPE		"Type"
#define MENU_TYPE_STR_POWER	"power_select"
#define MENU_TYPE_STR_H		"horizontal"
#define MENU_TYPE_STR_V		"vertical"
#define MENU_TYPE_STR_GRID	"grid"

typedef enum
{
	MENU_TYPE_NULL = 0x0,
	MENU_TYPE_H = 0x1,
	MENU_TYPE_V = 0x2,
	MENU_TYPE_GRID = 0x4,
	MENU_TYPE_POWER = 0x8,
	MENU_TYPE_MAX = 0x10

}menu_type_t;


typedef struct menu_item_s menu_item_t;
typedef struct menu_s menu_t;

struct menu_item_s
{
	menu_item_state_t	State;
	GameState			NextState;
	vec2_t				Position;
	char				*Name;
	sprite_t			*Image;
	void				*Info;
};

struct menu_s
{
	menu_item_t mItems[MENU_ITEM_MAX];
	menu_item_t *mSelectedItem;
	sprite_t *mBackground;
	GameState mCurrentState;
	GameState mPreviousState;
	void (*Update)(menu_t *self, SDL_GameControllerButton button);
	void (*Draw)(menu_t *self);
};

extern menu_t *gMenus;

int InitMenuSystem();
menu_t *LoadMenu(object_t *object, char *g_str, GameState curr_state, GameState previous_state );
menu_t *FindMenuFromGameState(GameState curr_state);
menu_t *FindFreeMenu();

#endif