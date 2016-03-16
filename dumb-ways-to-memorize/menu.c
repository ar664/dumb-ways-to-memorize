#include "globals.h"
#include "graphics.h"
#include "menu.h"
#include "mystrings.h"
#include <stdio.h>

menu_t *gMenus = NULL;

void UpdateVerticalMenu(menu_t *self, SDL_GameControllerButton button)
{
	if(!self)
	{
		printf("NULL menu updated \n");
		return;
	}
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_DPAD_UP):
		{
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			break;
		}
	default:
		{
			break;
		}
	}
}

void UpdatePowerUpMenu(menu_t *self, SDL_GameControllerButton button)
{
	if(!self)
	{
		printf("NULL menu updated \n");
		return;
	}
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_DPAD_UP):
		{
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			break;
		}
	default:
		{
			break;
		}
	}
}

/**
 * Allocate memor for menu system, gMenus is the menu list. 
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

void InitMenuSystem()
{
	if(gMenus)
	{
		printf("gMenus already initialized");
		return;
	}
	gMenus = (menu_t*) malloc(sizeof(menu_t)*MAX_MENUS);
	if(!gMenus)
	{
		printf("Allocate menu error");
		exitRequest = -1;
		return;
	}
	memset(gMenus, 0, sizeof(gMenus)*MAX_MENUS);
}

/**
 * Loads a menu.
 *
 * @param [in,out]	object 	If non-null, the object menu was parsed to.
 * @param [in,out]	g_str 	If non-null, the string.
 * @param	currState	  	State to assign the menu.
 * @param	previous_state	State before this menu.
 *
 * @return	0 if loads correctly, -1 if it doesn't.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

menu_t *LoadMenu(object_t* object, char *g_str ,GameState curr_state, GameState previous_state)
{
	menu_t *menu;
	jsmntok_t *temp_tok;
	object_t *temp_obj;
	char *temp_str, *type_str;
	int i, temp_i;

	//Check Vars given
	if(!object || !g_str)
	{
		printf("No object / str given to load menu \n");
		return NULL;
	}

	//Check menu type
	temp_str = FindValueFromKey(object->keys, MENU_TYPE, g_str);
	if(!temp_str)
	{
		printf("No menu type in menu found: %s \n", object->name);
		return NULL;
	}

	//Look for free menu slot
	menu = FindFreeMenu();
	if(!menu)
	{
		printf("Could not find free menu \n");
		return NULL;
	}

	menu->mPreviousState = previous_state;
	menu->mCurrentState = curr_state;

	//Load via Menu Type
	if(!strcmp(temp_str, "power_select"))
	{
		//Do power stuff here
		
	} else
	{
		temp_obj = FindObject(object, MENU_ITEMS);
		if(!temp_obj)
		{
			printf("No items in menu %s \n", object->name);
			return NULL;
		}
		temp_i = CountObjectChildren(temp_obj);
		for(i = 0; i < temp_i; i++)
		{
			temp_str = FindValueFromKey(temp_obj[i].keys, MENU_ITEM_SPRITE, g_str);
			if(temp_str)
			{
				menu->mItems[i].Image = LoadSprite(temp_str , 0);
			}
			temp_str = FindValueFromKey(temp_obj[i].keys, MENU_ITEM_TEXT, g_str);
			if(temp_str)
			{
				menu->mItems[i].Name = temp_str;
			}
			menu->mItems[i].State = MENU_ITEM_STATE_NOT_SELECTED;
			temp_str = FindValueFromKey(temp_obj[i].keys, MENU_ITEM_LINK, g_str);
			if(temp_str)
			{
				menu->mItems[i].NextState = StrToGameState(temp_str);
				free(temp_str);
			} else
			{
				menu->mItems[i].NextState = SPLASH;
			}
			menu->mItems[i].Info = NULL;
			
		}
	}


}

/**
 * Searches for the first menu from game state in gMenus.
 *
 * @param	curr_state	State of the curr.
 *
 * @return	null if it fails, else the found menu from game state.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

menu_t* FindMenuFromGameState(GameState curr_state)
{
	if(!gMenus)
	{
		printf("Menu Sys not Initialized");
		return NULL;
	}
}

menu_t* FindFreeMenu()
{
	int i;
	if(!gMenus)
	{
		printf("Menu System not initialized");
		return NULL;
	}

	for(i = 0; i < MAX_MENUS; i++)
	{
		if(!gMenus[i].Update)
		{
			return &gMenus[i];
		}
	}
	return NULL;
}
