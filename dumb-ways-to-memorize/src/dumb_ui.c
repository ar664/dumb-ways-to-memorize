#include "parseobject.h"
#include "dumb_ui.h"
#include "graphics.h"
#include "mystrings.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "dumb_physics.h"
#include "parselevel.h"
#include "player.h"

sprite_t *gUI_Sprite_Health;
vec2_t gUI_Health_Pos = {0,0};
vec2_t gUI_Power_Pos = {0,32};
menu_t *gMenus = NULL;
int gCurrentSelectedItem = 0;

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
	entity_t *temp_ent;
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
				break;
			}
			if(self->mSelectedItem <= self->mItems)
			{
				self->mSelectedItem = self->mItems;
				break;
			} else {
				self->mSelectedItem--;
			}
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			if(!self->mSelectedItem)
			{
				self->mSelectedItem = self->mItems;
				break;
			}
			if(self->mSelectedItem >= &self->mItems[self->mItemCount] ){
				self->mSelectedItem = &self->mItems[self->mItemCount];
				break;
			} else
			{
				self->mSelectedItem++; 
			}
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			if(!self->mSelectedItem)
			{
				self->mSelectedItem = self->mItems;
			}
			gGameState = self->mItems - &self->mSelectedItem[self->mItemCount];

			if( (gGameState == PLAYING) && self->mSelectedItem->Info)
			{
				if(!strcmp((char*)self->mSelectedItem->Info, MENU_EXTRA_LOAD_LEVEL))
				{
					printf("Loading save state \n");
					if(LoadGameState() == 0)
					{
						printf("Game Loaded Successfully! \n");
					} else
					{
						printf("Game could not be successfully loaded \n");
						gGameState = START;
						return;
					}
				}
			}
			if( gGameState == EDITOR)
			{
				gEditorEntity = (entity_t*) malloc(sizeof(entity_t));
				temp_ent = FindCachedEntity(PLAYER_STR);
				if(!temp_ent)
				{
					printf("No Player entity found, returning to main menu.");
					gGameState = START;
					return;
				}
				InitCursor();
				memcpy(gEditorEntity, temp_ent, sizeof(entity_t));
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
	object_t *temp_obj, *choose_obj;
	jsmntok_t *choose_tok;
	char *choose_data, *temp_str;
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

				//Get the currently chosen power ups.
				gSelectedPowerUps = (char**) malloc(sizeof(char*)*(powerUps+1));
				if(!gSelectedPowerUps)
				{
					break;
				}
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

				//Check if they correspond to the set amount of levels.
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

				//Allocate Memory for the Choose PowerUps Menu
				gUsedPowerUps = (char**) malloc(sizeof(char*)*(selected_power_ups+1));
				if(!gUsedPowerUps)
				{
					if(gSelectedPowerUps) free(gSelectedPowerUps);
					gSelectedPowerUps = NULL;
					break;
				}
				memset(gUsedPowerUps, 0, sizeof(char*)*(selected_power_ups+1));
				gSelectedPowerUps[selected_power_ups] = NULL;

				//Load the Choose Power Ups Menu
				temp_obj = FindObject(gGameObject, "Menus");
				if(!temp_obj)
				{
					if(gUsedPowerUps) free(gUsedPowerUps);
					gUsedPowerUps = NULL;
					if(gSelectedPowerUps) free(gSelectedPowerUps);
					gSelectedPowerUps = NULL;
					printf("Could not find menus in GameData \n");
					break;
				}
				temp_str = JsmnToString(&temp_obj->values[MENU_CHOOSE], gGameData);
				ConvertFileToUseable(temp_str, NULL, &choose_data, &choose_tok);
				choose_obj = ParseToObject(choose_tok, choose_data);
				if(!LoadMenu(choose_obj, choose_data, CHOOSE, GUESS))
				{
					printf("Failure to load Choose Menu \n");
					return;
				}

				//Reset the Power Up Menu
				for(i = 0; i < self->mItemCount; i++)
				{
					self->mItems[i].State = MENU_ITEM_STATE_NOT_SELECTED;
				}
				self->mSelectedItem = NULL;

				gGameState = CHOOSE;

				//Deallocation
				if(temp_str) free(temp_str);
				if(choose_data) free(choose_data);
				if(choose_tok) free(choose_tok);
				if(choose_obj) FreeObject(choose_obj);
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
				if(!gSelectedPowerUps)
				{
					printf("No power ups selected \n" );
					gGameState = GUESS;
					break;
				}
				powerUps = CountMem(gSelectedPowerUps, sizeof(char*));
				for(i = 0; i < powerUps; i++)
				{	
					if(gUsedPowerUps[i])
					{
						if(!strcmp(self->mSelectedItem->Name, gUsedPowerUps[i]))
						{
							printf("Power Up already used, choose another one. \n");
							break;
						}
					} else
					{	
						gUsedPowerUps[i] = self->mSelectedItem->Name;
						usedPower = 0;
						break;
					}
				}
				if(!usedPower)
				{
					gCurrentPowerUpName = self->mSelectedItem->Name;
					gCurrentPowerUp = FindPower(gCurrentPowerUpName);
					if(!gCurrentPowerUp)
					{
						printf("Could not find power up with name: %s. Please try again. \n", self->mSelectedItem->Name);
						break;
					}
					LoadSelectedLevel(i);
					gGameState = self->mSelectedItem->NextState;
					if(gGameState == PLAYING)
					{
						InitPlayer();
						InitCursor();
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
		if(DrawSprite(self->mBackground, NULL , NULL, gRenderer, 0))
		{
			printf("Failed to draw Background \n");
			return;
		}
	}

	for(i = 0; i < self->mItemCount; i++)
	{
		if( i == gCurrentSelectedItem )
		{
			if(DrawSprite(self->mItems[i].Image, NULL , &self->mItems[i].Position, gRenderer, 0))
			{
				printf("Failed to draw Menu Item : %d \n", i);
				return;
			}
			selection_rect.x = self->mItems[i].Position.x;
			selection_rect.y = self->mItems[i].Position.y;
			SDL_RenderCopy(gRenderer, gRedTexture, &selection_rect,&selection_rect);
		} else {
			if(DrawSprite(self->mItems[i].Image, NULL, &self->mItems[i].Position, gRenderer, 0))
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
		DrawSprite(self->mBackground, NULL , NULL, gRenderer, 0);
	}

	for(i = 0; i < self->mItemCount; i++)
	{
		if( &self->mItems[i] == self->mSelectedItem )
		{
			if(DrawSprite(self->mItems[i].Image, NULL , &self->mItems[i].Position, gRenderer, 0))
			{
				printf("Failed to draw Menu Item : %d \n", i);
				return;
			}
			selection_rect.x = self->mItems[i].Position.x;
			selection_rect.y = self->mItems[i].Position.y;
			SDL_RenderCopy(gRenderer, gRedTexture, &selection_rect,&selection_rect);
		} else
		{

			if(DrawSprite(self->mItems[i].Image, NULL , &self->mItems[i].Position, gRenderer, 0))
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

	//Look if this menu type was already loaded
	menu = FindMenuFromGameState(curr_state);
	if(menu)
	{
		printf("Free menu has ben called \n");
		FreeMenu(menu);
	} 
	else
	//Look for free menu slot if not found
	{

		menu = FindFreeMenu();
		if(!menu)
		{
			printf("Could not find free menu \n");
			return NULL;
		}
		//Setting to Zero just in case
		memset(menu, 0, sizeof(menu_t));
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
		printf("Not found menu layout type for %s. Switching to default vertical layout \n", object->name);
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
			if(temp_str) free(temp_str);
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
		} else
		{
			menu->mItems[i].Info = NULL;
		}
		
	}

	//Load extra items
	if(!strcmp(type_str, MENU_TYPE_STR_CHOOSE))
	{
		if(!gSelectedPowerUps)
		{
			printf("Trouble loading choose menu, no power ups selected \n");
			return NULL;
		}
		choose_i = CountMem(gSelectedPowerUps, sizeof(char*));
		ref_menu = FindMenuFromGameState(GUESS);
		if(!ref_menu)
		{
			printf("Trouble loading choose menu , no GUESS menu found \n");
			return NULL;
		}
		else
		{
			for(i = 0; i < choose_i; i++)
			{
				ref_menu_item = FindMenuItem(ref_menu, gSelectedPowerUps[i]);
				if(!ref_menu_item)
				{
					printf("Could not load powerup : %s \n", gSelectedPowerUps[i]);
					continue;
				}
				memcpy(&menu->mItems[menu->mItemCount+i], ref_menu_item, sizeof(menu_item_t));
				if(ref_menu_item->Name)
				{
					menu->mItems[menu->mItemCount+i].Name = strdup(ref_menu_item->Name);
				}
				if(ref_menu_item->Info)
				{
					menu->mItems[menu->mItemCount+i].Info = NULL;//_strdup((char*)ref_menu_item->Info);
				}
				
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
	if(!menuCmp)
	{
		return NULL;
	}
	memset(menuCmp, 0, sizeof(menu_t));
	for(i = 0; i < MAX_MENUS; i++)
	{
		if(!memcmp(menuCmp, &gMenus[i], sizeof(menu_t)))
		{
			if(menuCmp) free(menuCmp);
			return &gMenus[i];
		}
	}
	if(menuCmp) free(menuCmp);
	return NULL;
}

menu_item_t* FindMenuItem(menu_t* menu, char* item)
{
	int i, items;
	items = menu->mItemCount;
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

void FreeMenu(menu_t *menu)
{
	int i;
	if(!menu)
	{
		return;
	}
	if(menu->mBackground)
	{
		FreeSprite(menu->mBackground);
		menu->mBackground = NULL;
	}
	for(i = 0; i < MENU_ITEM_MAX; i++)
	{
		if(menu->mItems[i].Image)
		{
			if(gGameState == END)
			{
				FreeSprite(menu->mItems[i].Image);
			}
			menu->mItems[i].Image = NULL;
		}

		if(menu->mItems[i].Name)
		{
			free(menu->mItems[i].Name);
			menu->mItems[i].Name = NULL;
		}

		if(menu->mItems[i].Info)
		{
			free(menu->mItems[i].Info);
			menu->mItems[i].Info = NULL;
		}

	}
	memset(menu, 0, sizeof(menu_t));
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
int InitGUI()
{

	return 0;
}

void ShutdownGUI()
{
	return;
}

void LoadGUIforGameState(GameState game_state)
{
	char *temp_str;
	if(!gGameObject)
	{
		return;
	}
	switch(game_state)
	{
	case(PLAYING):
		{
			temp_str = FindValue(gGameObject, UI_PLAYER_STATUS, gGameData);
			if(!temp_str)
			{
				return;
			}
			gUI_Sprite_Health = LoadSprite(temp_str, 0);
			if(temp_str) free(temp_str);
		}
	default:
		break;
	}
}

void DrawUI()
{
	int i;
	vec2_t temp_pos;
	switch(gGameState)
	{
	case(PLAYING):
		{
			if(gUI_Sprite_Health)
			{
				for(i = 0; i < gPlayerLives; i++)
				{
					temp_pos.x = gUI_Health_Pos.x + i*32;
					temp_pos.y = gUI_Health_Pos.y;
					DrawSprite(gUI_Sprite_Health, NULL, &temp_pos, gRenderer, 0);
				}
				

			}
			if(gCurrentPowerUp)
			{
				if(gCurrentPowerUp->info)
				{
					if(gCurrentPowerUp->info->mSprites[0] )
					{
						temp_pos.x = gUI_Power_Pos.x;
						temp_pos.y = gUI_Power_Pos.y;
						DrawSprite(gCurrentPowerUp->info->mSprites[0], NULL, &temp_pos, gRenderer, 0);
					}
				}
				
			}
			
			break;
		}
	default:
		break;
	}
}
