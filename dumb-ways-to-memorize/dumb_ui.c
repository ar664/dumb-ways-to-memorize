#include "parseobject.h"
#include "mystrings.h"
#include "dumb_ui.h"

sprite_t *gUI_Sprite_Health;
vec2_t gUI_Health_Pos = {0,0};
vec2_t gUI_Power_Pos = {0,32};

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
			break;
		}
	default:
		break;
	}
}