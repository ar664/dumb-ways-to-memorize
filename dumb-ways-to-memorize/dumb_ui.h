#ifndef __DUMB_UI
#define __DUMB_UI

#include "graphics.h"

typedef struct element_s element_t;
typedef struct window_s window_t;

//Hard-coding some shit
#define UI_PLAYER_STATUS "UI_Lives"

/**
 * An element of the UI for the game.
 *
 * @author	Anthony Rios
 * @date	4/27/2016
 */

struct element_s
{
	menu_item_state_t mState;
	ui_type_t mType;
	area_t mPosition;   /**< The position/size of the element on a percentage of the area its assigned to */
	sprite_t *mSprite;
	void *mData;	/**< The data necessary to use this element.
					 *	This is based on type what will be there.
					 *	If the type is panel : entity list with NULL end is expected.
					 *  Label : char* to text
					 *  Button : func* to a more specific function
					 *  Image : none
					 */
	char *mName;

	void (*Draw)(element_t *self);
	void (*Think)(element_t *self);
};

struct window_s
{
	window_t *mParent;
	area_t mPosition;   /**< The position/size of the window in percentage of parents position */
	element_t *elements;
};

int InitGUI();
void ShutdownGUI();

void LoadGUIforGameState(GameState game_state);
void DrawUI();

extern sprite_t *gUI_Sprite_Health;
extern vec2_t gUI_Health_Pos;
extern vec2_t gUI_Power_Pos;


#endif