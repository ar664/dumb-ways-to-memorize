#include "globals.h"
#include "graphics.h"
#include "menu.h"
#include "mystrings.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "parsepowerup.h"
#include "parselevel.h"
#include "player.h"

menu_t *gMenus = NULL;
int gCurrentSelectedItem = 0;

/**
 * Deselect item by number.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param	item			The item number to deselect.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void DeselectItemByNum(menu_t *self, int item)
{
	if(!self)
	{
		printf("Deselect item not given menu");
		return;
	}
	self->mItems[item].State = MENU_ITEM_STATE_NOT_SELECTED;
}

/**
 * Select item by number.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param	item			The item number to select.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void SelectItemByNum(menu_t *self, int item)
{
	if(!self)
	{
		printf("Select item not given menu");
		return;
	}
	self->mItems[item].State = MENU_ITEM_STATE_SELECTED;
}

/**
 * Uses gSelectedItem. Updates input and state of menu items.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param	button			The button.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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
			if(!gCurrentSelectedItem)
			{
				break;
			}
			DeselectItemByNum(self, gCurrentSelectedItem);
			if(gCurrentSelectedItem-1 < 0)
			{
				SelectItemByNum(self, gCurrentSelectedItem);
				break;
			}
			gCurrentSelectedItem--;
			SelectItemByNum(self, gCurrentSelectedItem);
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			DeselectItemByNum(self, gCurrentSelectedItem);
			if(gCurrentSelectedItem+1 == CountMem(self->mItems, sizeof(menu_item_t)))
			{
				SelectItemByNum(self, gCurrentSelectedItem);
				break;
			}
			gCurrentSelectedItem++;
			SelectItemByNum(self, gCurrentSelectedItem);
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			gGameState = self->mItems[gCurrentSelectedItem].NextState;
			break;
		}
	default:
		{
			break;
		}
	}
}

/**
 * Uses mSelectedItem. Updates the menu's selected item, and checks for number of power ups selected, to
 * determine whether to switch game states.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param	button			The button.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void UpdatePowerUpMenu(menu_t *self, SDL_GameControllerButton button)
{
	int i, powerUps, selected_power_ups;
	object_t *temp, *choose_obj;
	jsmntok_t *choose_tok;
	char *choose_data;
	if(!self)
	{
		printf("NULL menu updated \n");
		return;
	}
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_DPAD_UP):
		{
			if(!self->mSelectedItem)
			{
				self->mSelectedItem = self->mItems;
			}
			if(self->mSelectedItem-1 < self->mItems)
			{
				break;
			}
			self->mSelectedItem--;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			if(!self->mSelectedItem)
			{
				self->mSelectedItem = self->mItems;
			}
			if(self->mSelectedItem+1 == self->mItems+self->mItemCount)
			{
				break;
			}
			self->mSelectedItem++;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			if(!self->mSelectedItem)
			{
				self->mSelectedItem = self->mItems;
			}
			if(self->mSelectedItem->NextState == SPLASH)
			{
				self->mSelectedItem->State = (menu_item_state_t) (~self->mSelectedItem->State & MENU_ITEM_STATE_PICKED);
			} else
			{
				powerUps = CountMem(gPowerUps, sizeof(power_t));
				gSelectedPowerUps = (char**) malloc(sizeof(char*)*(powerUps+1));
				memset(gSelectedPowerUps, 0, sizeof(char*)*(powerUps+1));
				selected_power_ups = 0;
				for(i = 0; i < powerUps; i++)
				{
					if(self->mItems[i].State & MENU_ITEM_STATE_PICKED)
					{
						gSelectedPowerUps[selected_power_ups] = gPowerUps[i].name;
						selected_power_ups++;
					}
				}
				if(selected_power_ups != gLevelsPerGame)
				{
					printf("Too many or not enough power_ups selected");
					for(i = 0; i < selected_power_ups; i++)
					{
						gSelectedPowerUps[i] = NULL;
					}
					if(gSelectedPowerUps) free(gSelectedPowerUps);
					break;
				}
				gUsedPowerUps = (char**) malloc(sizeof(char*)*(selected_power_ups+1));
				memset(gUsedPowerUps, 0, sizeof(char*)*(selected_power_ups+1));
				gSelectedPowerUps[selected_power_ups] = NULL;
				temp = FindObject(gGameObject, "Menus");
				ConvertFileToUseable(JsmnToString(&temp->values[2], gGameData), NULL, &choose_data, &choose_tok);
				choose_obj = ParseToObject(choose_tok, choose_data);
				if(!LoadMenu(choose_obj, choose_data, CHOOSE, GUESS))
				{
					printf("Failure to load Choose Menu \n");
					return;
				}
				for(i = 0; i < self->mItemCount; i++)
				{
					self->mItems[i].State = MENU_ITEM_STATE_NOT_SELECTED;
				}
				self->mSelectedItem = NULL;
				gGameState = CHOOSE;
			}
			break;
		}
	default:
		{
			break;
		}
	}
}

/**
 * Updates the power select menu, sets current powerup of the player.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param	button			The button.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void UpdatePowerSelectMenu(menu_t* self, SDL_GameControllerButton button)
{
	int i, powerUps, usedPower = 1;
	if(!self)
	{
		printf("NULL menu updated \n");
		return;
	}
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_DPAD_UP):
		{
			if(self->mSelectedItem-1 < self->mItems)
			{
				break;
			}
			self->mSelectedItem--;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			if(self->mSelectedItem+1 == self->mItems+self->mItemCount)
			{
				break;
			}
			self->mSelectedItem++;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			if(self->mSelectedItem->State != MENU_ITEM_STATE_NULL)
			{
				powerUps = CountMem(gSelectedPowerUps, sizeof(char*));
				for(i = 0; i < powerUps; i++)
				{
					if(!gUsedPowerUps[i])
					{
						gUsedPowerUps[i] = self->mSelectedItem[i].Name;
						usedPower = 0;
						break;
					} else if(!strcmp(self->mSelectedItem[i].Name, gUsedPowerUps[i]))
					{
						break;
					}
				}
				if(!usedPower)
				{
					gCurrentPowerUpName = self->mSelectedItem[i].Name;
					gCurrentPowerUp = FindPower(gCurrentPowerUpName);
					
					LoadSelectedLevel(i);
					gGameState = self->mSelectedItem[i].NextState;
					if(gGameState == PLAYING)
					{
						InitPlayer();
					}
				}
			}
		}
	default:
		break;
	}
	
}

int InitMenuSystem()
{
	if(gMenus)
	{
		printf("gMenus already initialized");
		return -1;
	}
	gMenus = (menu_t*) malloc(sizeof(menu_t)*MAX_MENUS);
	if(!gMenus)
	{
		printf("Allocate menu error");
		return -1;
	}
	memset(gMenus, 0, sizeof(menu_t)*MAX_MENUS);
	return 0;
}

//Draw Function through gCurrentSelectedItm
void DrawMenuByNum(menu_t *self)
{
	int i;
	SDL_Rect selection_rect = {0,0,10,10};

	if(!self)
	{
		printf("Null menu tried to be drawn \n");
		return;
	}

	if(self->mBackground)
	{
		if(DrawSprite(self->mBackground, NULL , NULL, gRenderer))
		{
			printf("Failed to draw Background \n");
			return;
		}
	}

	for(i = 0; i < self->mItemCount; i++)
	{
		if( i == gCurrentSelectedItem )
		{
			if(DrawSprite(self->mItems[i].Image, NULL , &self->mItems[i].Position, gRenderer))
			{
				printf("Failed to draw Menu Item : %d \n", i);
				return;
			}
			selection_rect.x = self->mItems[i].Position.x;
			selection_rect.y = self->mItems[i].Position.y;
			SDL_RenderCopy(gRenderer, gRedTexture, &selection_rect,&selection_rect);
		} else {
			if(DrawSprite(self->mItems[i].Image, NULL, &self->mItems[i].Position, gRenderer))
			{
				printf("Failed to draw Menu Item : %d \n", i);
				return;
			}
		}
	}

}

//Draw Function through mItem State
void DrawMenuByState(menu_t *self)
{
	int i;
	SDL_Rect selection_rect = {0,0,10,10};

	if(!self)
	{
		printf("Null menu tried to be drawn \n");
		return;
	}

	if(self->mBackground)
	{
		DrawSprite(self->mBackground, NULL , NULL, gRenderer);
	}

	for(i = 0; i < self->mItemCount; i++)
	{
		if( &self->mItems[i] == self->mSelectedItem )
		{
			if(DrawSprite(self->mItems[i].Image, NULL , &self->mItems[i].Position, gRenderer))
			{
				printf("Failed to draw Menu Item : %d \n", i);
				return;
			}
			selection_rect.x = self->mItems[i].Position.x;
			selection_rect.y = self->mItems[i].Position.y;
			SDL_RenderCopy(gRenderer, gRedTexture, &selection_rect,&selection_rect);
		} else
		{

			if(DrawSprite(self->mItems[i].Image, NULL , &self->mItems[i].Position, gRenderer))
			{
				printf("Failed to draw Menu Item : %d \n", i);
				return;
			}
		}
		if( self->mItems[i].State & MENU_ITEM_STATE_PICKED )
		{
			selection_rect.x = self->mItems[i].Position.x + self->mItems[i].Image->mSize.x;
			selection_rect.y = self->mItems[i].Position.y;
			SDL_RenderCopy(gRenderer, gRedTexture, &selection_rect,&selection_rect);
		}
	}

}

/**
 * Sets the positions of the menu items based on the type of layout.
 *
 * @param [in,out]	items	If non-null, the items.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/16/2016
 */

void ProcessMenuItemsByType(menu_item_t *items,menu_type_t type)
{
	SDL_Rect format;
	int itemCount, i, width, height, divisor;
	float radius;
	itemCount = CountMem(items, sizeof(menu_item_t));
	divisor = 0; radius = 0;
	width = gScreenWidth ? gScreenWidth : SCREEN_RES_W;
	height = gScreenHeight ? gScreenHeight : SCREEN_RES_H;


	switch(type)
	{
	case(MENU_TYPE_H):
		{
			SDL_SetRect(&format, 0, height/(itemCount+1), 0, 0);
			break;
		}
	case(MENU_TYPE_V) : 
		{
			SDL_SetRect(&format, width/(itemCount+1), 0, 0, 0);
			break;
		}
	case(MENU_TYPE_GRID):
		{
			SDL_SetRect(&format, width/(itemCount+1), height/(itemCount+1), 0, 0);
			divisor = LargestDivisor(itemCount);
			break;
		}
	case(MENU_TYPE_POWER):
		{
			//Center of Screen
			SDL_SetRect(&format, width/2, height/2, 0, 0);
			radius = (float) height/3;
			break;
		}
	default:
		{
			SDL_SetRect(&format, 0, height/(itemCount+1), 0, 0);
			break;
		}
	}
	
	for(i = 0; i < itemCount; i++)
	{
		if(type & (MENU_TYPE_H | MENU_TYPE_V | MENU_TYPE_CHOOSE))
		{
			items[i].Position.x = format.x * i;
			items[i].Position.y = format.y * i;
		}
		if(type & MENU_TYPE_GRID)
		{
			items[i].Position.x = format.x * i%divisor;
			items[i].Position.y = format.y * i/divisor;
		}
		if(type & MENU_TYPE_POWER)
		{
			items[i].Position.x = format.x - (float) radius*sinf( 3.14 * (float) i/(itemCount+1));
			items[i].Position.y = format.y - (float) radius*cosf( 3.14 * (float) i/(itemCount+1));
		}
	}

	
}

menu_t *LoadMenu(object_t* object, char *g_str ,GameState curr_state, GameState previous_state)
{
	menu_t *menu, *ref_menu;
	menu_item_t *ref_menu_item;
	object_t *temp_obj;
	char *temp_str, *type_str, *bg_str;
	int i, temp_i, choose_i;

	//Check Vars given
	if(!object || !g_str)
	{
		printf("No object / str given to load menu \n");
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

	temp_obj = FindObject(object, MENU_ITEMS);
	if(!temp_obj)
	{
		printf("No items in menu %s \n", object->name);
		return NULL;
	}

	//Load Background
	bg_str = FindValue(object, MENU_BACKGROUND, g_str);
	if(!bg_str)
	{
		printf("No background loaded for menu %s \n", object->name);
	} else 
	{
		menu->mBackground = LoadSprite(bg_str, 0);
	}

	//Check menu type
	type_str = FindValue(object, MENU_TYPE, g_str);
	if(!type_str)
	{
		printf("Not found menu layout type for %s. Switching to default vertical layout", object->name);
		type_str = strdup(MENU_TYPE_STR_V);
	}

	//Load Items
	temp_i = CountObjectChildren(temp_obj);
	if(temp_i >= MENU_ITEM_MAX)
	{
		printf("Max menu items for menu %s \n", object->name);
		temp_i = MENU_ITEM_MAX -1;
	}

	menu->mItemCount = temp_i;
	for(i = 0; i < temp_i; i++)
	{
		menu->mItems[i].State = MENU_ITEM_STATE_NOT_SELECTED;
		menu->mItems[i].Info = NULL;

		temp_str = FindValue(&temp_obj->children[i], MENU_ITEM_SPRITE, g_str);
		if(temp_str)
		{
			menu->mItems[i].Image = LoadSprite(temp_str, 0);
		}

		temp_str = FindValue(&temp_obj->children[i], MENU_ITEM_TEXT, g_str);
		if(temp_str)
		{
			menu->mItems[i].Name = temp_str;
		}

		temp_str = FindValue(&temp_obj->children[i], MENU_ITEM_LINK, g_str);
		if(temp_str)
		{
			menu->mItems[i].NextState = StrToGameState(temp_str);
			if(temp_str) free(temp_str);
		} else
		{
			menu->mItems[i].NextState = SPLASH;
		}

		temp_str = FindValue(&temp_obj->children[i], MENU_ITEM_EXTRA, g_str);
		if(temp_str)
		{
			menu->mItems[i].Info = temp_str;
			if(!menu->mItems[i].Name)
			{
				menu->mItems[i].Name = temp_str;
			}
		} else
		{
			menu->mItems[i].Info = NULL;
		}
		
	}

	//Load extra items
	if(!strcmp(type_str, MENU_TYPE_STR_CHOOSE))
	{
		
		choose_i = CountMem(gSelectedPowerUps, sizeof(char*));
		ref_menu = FindMenuFromGameState(GUESS);
		if(!ref_menu)
		{
			printf("Trouble loading choose menu , no GUESS menu found \n");
		}
		else
		{
			for(i = 0; i < choose_i; i++)
			{
				ref_menu_item = FindMenuItem(ref_menu, gSelectedPowerUps[i]);
				if(!ref_menu_item)
				{
					continue;
				}
				memcpy(&menu->mItems[menu->mItemCount+i], ref_menu_item, sizeof(menu_item_t));
				menu->mItems[menu->mItemCount+i].State = MENU_ITEM_STATE_NOT_SELECTED;
				menu->mItems[menu->mItemCount+i].NextState = PLAYING;
			}
			menu->mItemCount += choose_i;
		}
		
	}

	//Calculate Menu Item Positions
	ProcessMenuItemsByType(menu->mItems, (menu_type_t) StrToMenuType(type_str));
	menu->mSelectedItem = menu->mItems;
	menu->mSelectedItem->State = MENU_ITEM_STATE_SELECTED;
	//gCurrentSelectedItem = 0;

	//Set Up Different Menu Types
	if(!strcmp(type_str, MENU_TYPE_STR_V))
	{
		menu->Update = UpdateVerticalMenu;
		menu->Draw = DrawMenuByNum;
	} else if(!strcmp(type_str, MENU_TYPE_STR_POWER))
	{
		menu->Update = UpdatePowerUpMenu;
		menu->Draw = DrawMenuByState;
	} else if (!strcmp(type_str, MENU_TYPE_STR_CHOOSE))
	{
		menu->Update = UpdatePowerSelectMenu;
		menu->Draw = DrawMenuByState;
	} else
	{
		menu->Update = UpdateVerticalMenu;
		menu->Draw = DrawMenuByNum;
	}

	return menu;

}

menu_t* FindMenuFromGameState(GameState curr_state)
{
	int i;
	if(!gMenus)
	{
		printf("Menu Sys not Initialized");
		return NULL;
	}
	for(i = 0; i < MAX_MENUS; i++)
	{
		if(!gMenus[i].Update)
		{
			continue;
		}
		if(gMenus[i].mCurrentState == curr_state)
		{
			return &gMenus[i];
		}
	}
	return NULL;
}

menu_t* FindFreeMenu()
{
	int i, *menuCmp;
	if(!gMenus)
	{
		printf("Menu System not initialized");
		return NULL;
	}
	menuCmp = (int*) malloc(sizeof(menu_t));
	memset(menuCmp, 0, sizeof(menu_t));
	for(i = 0; i < MAX_MENUS; i++)
	{
		if(!memcmp(menuCmp, &gMenus[i], sizeof(menu_t)))
		{
			return &gMenus[i];
		}
	}
	return NULL;
}

menu_item_t* FindMenuItem(menu_t* menu, char* item)
{
	int i, items;
	items = CountMem(menu->mItems, sizeof(menu_item_t));
	for(i = 0; i < items; i++)
	{
		if(!menu->mItems[i].Name)
		{
			continue;
		}
		if(!strcmp(item, menu->mItems[i].Name))
		{
			return &menu->mItems[i];
		}
	}
	return NULL;
}

int StrToMenuType(char *str)
{
	if(!strcmp(str, MENU_TYPE_STR_H))
	{
		return MENU_TYPE_H;
	}
	if(!strcmp(str, MENU_TYPE_STR_V))
	{
		return MENU_TYPE_V;
	}
	if(!strcmp(str, MENU_TYPE_STR_GRID))
	{
		return MENU_TYPE_GRID;
	}
	if(!strcmp(str, MENU_TYPE_STR_POWER))
	{
		return MENU_TYPE_POWER;
	}
	if(!strcmp(str, MENU_TYPE_STR_CHOOSE))
	{
		return MENU_TYPE_CHOOSE;
	}
	return MENU_TYPE_NULL;
}
