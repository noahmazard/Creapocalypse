#include "inputs.h"
#include "camera.h"
#include "player.h"
#include "monster.h"
#include "core.h"
#include "tower.h"
#include "init.h"
#include "highScore.h"

void GetWindowsInputs(GameData* _gd)
{
	sfEvent event;
	while (sfRenderWindow_pollEvent(_gd->window, &event))
	{
		if (event.type == sfEvtClosed) exit(0);
		if (event.type == sfEvtMouseButtonPressed)
		{
			//L-Shift + LMouseButton : Add Move Action
			if (event.mouseButton.button == sfMouseLeft && sfKeyboard_isKeyPressed(sfKeyLShift))
			{
				sfVector2i mousePos = sfMouse_getPositionRenderWindow(_gd->window);
				sfVector2f worldPos = ScreenToWorld(mousePos.x, mousePos.y, _gd->camera);

				//if aimed block is walkable :compute pathfinding and add the actions
				if (IsBlockWalkable(worldPos.x, worldPos.y, 0, 0, _gd))
				{
					Action* action = _gd->player.actions;
					//if player has actions, add it to the action list
					if (action)
					{
						while (action->next)
						{
							action = action->next;
						}
						A_Star(worldPos, action->pos, _gd);
					}
					//else do it now
					else
					{
						sfVector2f playerPos = { _gd->player.x, _gd->player.y };
						A_Star(worldPos, playerPos, _gd);
					}
				}
			}

			//R-MouseButton : Add build action
			if (event.mouseButton.button == sfMouseRight)
			{
				sfVector2i mousePos = sfMouse_getPositionRenderWindow(_gd->window);
				sfVector2f worldPos = ScreenToWorld(mousePos.x, mousePos.y, _gd->camera);

				//if had the enought money and aimed bloc is walkable : add build to next actions
				if (_gd->money >= GetTowerPrice(_gd->toBuild) && IsBlockWalkable(worldPos.x, worldPos.y, 0, 0, _gd))
				{
					int succes;
					Action* action = _gd->player.actions;

					//if L-Shift pressed, compute path from the last action of the list
					if (sfKeyboard_isKeyPressed(sfKeyLShift) && action)
					{
						while (action->next)
						{
							action = action->next;
						}
						succes = A_Star(worldPos, action->pos, _gd);
					}
					//if unpressed, clear action list and compute pathfinding from player position
					else
					{
						sfVector2f playerPos = { _gd->player.x, _gd->player.y };
						ClearPlayerActions(&_gd->player, _gd);
						succes = A_Star(worldPos, playerPos, _gd);
					}

					//if pathfinding doesn't fail, add the next path and action
					if (succes)
					{
						Action* newAction = CreateBuildAction(worldPos, _gd->toBuild, _gd);
						AddPlayerAction(&_gd->player, newAction);
					}
				}
			}
		}
		if (event.type == sfEvtKeyPressed)
		{
			//Escape : Quit the game
			if (event.key.code == sfKeyEscape)
			{
				exit(0);
			}
			//1 to 5 : Select turret
			if (event.key.code == sfKeyNum1)
			{
				_gd->toBuild = TURRET1;
			}
			if (event.key.code == sfKeyNum2)
			{
				_gd->toBuild = TURRET2;
			}
			if (event.key.code == sfKeyNum3)
			{
				_gd->toBuild = TURRET3;
			}
			if (event.key.code == sfKeyNum4)
			{
				_gd->toBuild = TURRET4;
			}
			if (event.key.code == sfKeyNum5)
			{
				_gd->toBuild = WALL;
			}

			// === DEBUGING KEYS === //
			//F1 : set to debug mod (show path)
			if (event.key.code == sfKeyF1)
			{
				_gd->debug = (_gd->debug + 1) % 2;
			}
			//F2 : spawn a monster at the right down corner of the map
			if (event.key.code == sfKeyF2)
			{
				SpawnMonster(_gd, EXPLOSIVE_MONSTER, (sfVector2f) {240, 240});
			}
			//F3 : print the tower binary tree in the terminal
			if (event.key.code == sfKeyF3) 
			{
				printf("- - - - - - - -  \n");
				PrintfTowers(_gd->tower, _gd, 0);
				printf("- - - - - - - -  \n");
			}
			//F4 : Reload Game
			if (event.key.code == sfKeyF4)
			{
				UnloadGameData(_gd);
				LoadGameData(_gd);
			}
			//F5 : kill the player
			if (event.key.code == sfKeyF5)
			{
				_gd->player.hp = 0;
			}
		}
	}

	//Update DT
	_gd->dt = sfTime_asSeconds(sfClock_restart(_gd->clock));
	//check the max DT to avoid some bugs
	_gd->dt = _gd->dt > DTMAX ? DTMAX : _gd->dt;  
}

void GetPlayerInputs(GameData* _gd)
{
	//if has focus to avoid action while clicking out of the screen
	if (sfRenderWindow_hasFocus(_gd->window))
	{
		//L-MouseButton : Move to the mouse position
		if (sfMouse_isButtonPressed(sfMouseLeft) && !sfKeyboard_isKeyPressed(sfKeyLShift))
		{
			sfVector2i mousePos = sfMouse_getPositionRenderWindow(_gd->window);
			sfVector2f worldPos = ScreenToWorld(mousePos.x, mousePos.y, _gd->camera);

			//if the aimed block is walkable : clear the action list and move to the position
			if (IsBlockWalkable(worldPos.x, worldPos.y, 1, 0,_gd))
			{
				sfVector2f playerPos = { _gd->player.x, _gd->player.y };
				ClearPlayerActions(&_gd->player, _gd);
				A_Star(worldPos, playerPos, _gd);
			}
		}

		//ZQSD : move the camera
		if (sfKeyboard_isKeyPressed(sfKeyZ))
		{
			_gd->camera.y -= SPEED * TILE_SIZE * 2 * _gd->dt;
			_gd->cameraMode = CAMERA_FREE;
		}
		if (sfKeyboard_isKeyPressed(sfKeyQ))
		{
			_gd->camera.x -= SPEED * TILE_SIZE * 2 * _gd->dt;
			_gd->cameraMode = CAMERA_FREE;
		}
		if (sfKeyboard_isKeyPressed(sfKeyS))
		{
			_gd->camera.y += SPEED * TILE_SIZE * 2 *  _gd->dt;
			_gd->cameraMode = CAMERA_FREE;
		}
		if (sfKeyboard_isKeyPressed(sfKeyD))
		{
			_gd->camera.x += SPEED * TILE_SIZE * 2 * _gd->dt;
			_gd->cameraMode = CAMERA_FREE;
		}
	}
}

//Get all inputs of the game
void GetInputs(GameData* _gd)
{
	if (_gd->player.hp > 0)
	{
		GetWindowsInputs(_gd);
		GetPlayerInputs(_gd);
	}
	else
	{
		GetHighscoreInputs(_gd);
	}
}