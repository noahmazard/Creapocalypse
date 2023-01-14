#include "update.h"
#include "myMath.h"
#include "player.h"
#include "camera.h"
#include "monster.h"
#include "building.h"
#include "core.h"
#include "tower.h"

void UpdateCamera(GameData* _gd)
{
	switch (_gd->cameraMode)
	{
	//Set the camera pos to the player pos
	case CAMERA_PLAYER:
		_gd->camera.x = _gd->player.x * TILE_SIZE - WINDOW_WIDTH / 2;
		_gd->camera.y = _gd->player.y * TILE_SIZE - WINDOW_HEIGHT / 2;
		break;
	//CAMERA_FREE movement are in input.c
	case CAMERA_FREE:
		break;
	default:
		break;
	}

	//if Camera out of bounds, set it to bounds
	_gd->camera.x = _gd->camera.x < 0 ? 0 : _gd->camera.x;
	_gd->camera.x = _gd->camera.x > TILE_SIZE * MAP_WIDTH - WINDOW_WIDTH ? 
									TILE_SIZE * MAP_WIDTH - WINDOW_WIDTH : _gd->camera.x;

	_gd->camera.y = _gd->camera.y < 0 ? 0 : _gd->camera.y;
	_gd->camera.y = _gd->camera.y > TILE_SIZE * MAP_HEIGHT - WINDOW_HEIGHT ?
									TILE_SIZE * MAP_HEIGHT - WINDOW_HEIGHT : _gd->camera.y;
}

//The debug HUD draw a line on screen for each of the next player moves
void UpdateDebugHud(GameData* _gd)
{
	Action* action = _gd->player.actions;
	if (action && _gd->debug)
	{
		Action* previous = action;
		sfVector2i pPos = WorldToScreen(_gd->player.x, _gd->player.y, _gd->camera);
		sfVector2i aPos = WorldToScreen(action->pos.x, action->pos.y, _gd->camera);

		//reset all pixel to transparent
		for (int x = 0; x < WINDOW_WIDTH; x++)
		{
			for (int y = 0; y < WINDOW_HEIGHT; y++)
			{
				if (sfImage_getPixel(_gd->hudImage, x, y).a)
				{
					DrawPixel(_gd->hudImage, x, y, sfColor_fromRGBA(0, 0, 0, 0));
				}
			}
		}

		//Draw the first action
		DrawLine(pPos.x, pPos.y, aPos.x, aPos.y, sfRed, _gd->hudImage);

		//For each of the others actions, draw it
		action = previous->next;
		while (action)
		{
			sfVector2i pPos = WorldToScreen(previous->pos.x, previous->pos.y, _gd->camera);
			sfVector2i aPos = WorldToScreen(action->pos.x, action->pos.y, _gd->camera);

			DrawLine(pPos.x, pPos.y, aPos.x, aPos.y, sfRed, _gd->hudImage);
			previous = action;
			action = previous->next;
		}

		sfTexture_updateFromImage(sfSprite_getTexture(_gd->hud), _gd->hudImage, 0, 0);
	}
}

//Update all the game
void Update(GameData* _gd)
{
	if (_gd->player.hp > 0)
	{
		UpdateCamera(_gd);
		UpdatePlayer(_gd);
		UpdateBuildings(_gd);
		UpdtateTowers(_gd->tower, _gd);
		UpdateShot(_gd);
		UpdateMonsters(_gd);
		UpdateMonsterShot(_gd);
		UpdateDebugHud(_gd);
	}
}