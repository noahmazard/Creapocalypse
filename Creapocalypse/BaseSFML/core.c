#include "core.h"
#include "myMath.h"
#include "tower.h"

//Simualte a linear move and return a the first collision point, -1, -1 if none
sfVector2f GetPathCollision(sfVector2f _startPos, sfVector2f _aimedPos, GameData* _gd)
{
	//if start on screen
	if (_startPos.x < MAP_WIDTH && _startPos.y < MAP_HEIGHT && _startPos.x >= 0 && _startPos.y >= 0)
	{

		sfVector2f currentPos = _startPos;
		sfVector2f move = GetVector(_startPos, _aimedPos);
		move = NormalizeVector(move);
		currentPos.x += move.x * 1.5f;
		currentPos.y += move.y * 1.5f;
		
		//while pos < 0.5 bloc : move forward 0.1 bloc
		while (fabs(currentPos.x - _aimedPos.x) >= 0.5f || fabs(currentPos.y - _aimedPos.y) >= 0.5f)
		{
			currentPos.x += move.x * 0.1f;
			currentPos.y += move.y * 0.1f;
			//if block is not walkable : return collision
			if (!IsBlockWalkable(currentPos.x, currentPos.y, 1, 1, _gd))
			{
				currentPos.x -= move.x * 0.1f;
				currentPos.y -= move.y * 0.1f;
				currentPos.x = (int)currentPos.x + 0.5f;
				currentPos.y = (int)currentPos.y + 0.5f;
				return currentPos;
			}
			//if distance > 250, out of map so return to avoid infinite loop
			if (fabs(currentPos.x - _aimedPos.x) >= 250.f || fabs(currentPos.y - _aimedPos.y) >= 250.f)
			{
				return (sfVector2f) { -1, -1 };
			}
		}
	}
	//return no collision
	return (sfVector2f) { -1, -1 };
}
//Check if a block is walkable or not
char IsBlockWalkable(int _x, int _y, char _isPreviewWalkable, char _canOutOfMap ,GameData* _gd)
{
	if (_x >= MAP_WIDTH || _y >= MAP_HEIGHT || _x < 0 || _y < 0)
	{
		return _canOutOfMap;
	}
	return _gd->map[_y][_x].type < PREVIEW + _isPreviewWalkable;
}
//Update the map and set a certain bloc
void SetBlock(int _x, int _y, char _type, void* _build, GameData* _gd)
{
	//if the bloc was a preview, free his data
	if (_gd->map[_y][_x].type == PREVIEW && _gd->map[_y][_x].build != NULL)
	{
		Tower* tower = (Tower*)_gd->map[_y][_x].build;
		DeleteTower(tower, _gd);
	}

	//if the bloc will be a tower : create his data and insert it in the linked list
	if (_build == NULL && _type >= WALL)
	{
		_build = malloc(sizeof(Tower));
		Tower* tower = (Tower*)_build;
		tower->up = NULL;
		tower->down = NULL;
		tower->previous = NULL;
		tower->x = _x;
		tower->y = _y;
		tower->type = _type;
		tower->isPreview = 0;
		tower->lastShot = 0;
		switch (_type)
		{
		case WALL:
			tower->hp = HP_WALL;
			tower->hpMax = HP_WALL;
			break;
		case TURRET1:
			tower->hp = HP_TOWER1;
			tower->hpMax = HP_TOWER1;
			tower->cd = 1;
			break;
		case TURRET2:
			tower->hp = HP_TOWER2;
			tower->hpMax = HP_TOWER2;
			tower->cd = 1;
			break;
		case TURRET3:
			tower->hp = HP_TOWER3;
			tower->hpMax = HP_TOWER3;
			tower->cd = 1.5f;
			break;
		case TURRET4:
			tower->hp = HP_TOWER4;
			tower->hpMax = HP_TOWER4;
			tower->cd = 3;
			break;
		default:
			tower->hp = -1;
			tower->hpMax = -1;
			break;
		}
		if (tower->type >= TURRET1)
		{
			InsertTower(&_gd->tower, tower);
		}
	}

	//Change the bloc properties
	_gd->map[_y][_x].build = _build;
	_gd->map[_y][_x].type = _type;
}

//Like SetBlock but specialized for the preview
void SetBlockPreview(int _x, int _y, char _type, GameData* _gd)
{
	void* _build = malloc(sizeof(Tower));
	((Tower*)_build)->up = NULL;
	((Tower*)_build)->down = NULL;
	((Tower*)_build)->previous = NULL;
	((Tower*)_build)->x = _x;
	((Tower*)_build)->y = _y;
	((Tower*)_build)->type = _type;
	((Tower*)_build)->isPreview = 1;
	InsertTower(&_gd->tower, _build);
	SetBlock(_x, _y, PREVIEW, _build, _gd);
}

//Inflict Damage to a block
void DamageBlock(int _x, int _y, int _damages, GameData* _gd)
{
	//check ifvalid coordinates
	if (_x < MAP_WIDTH && _y < MAP_HEIGHT && _x >= 0 && _y >= 0)
	{
		//if part of a building, inflict damages to the coresponding build
		if (_gd->map[_y][_x].type == BUILDING)
		{
			Buildings* build = (Buildings*)_gd->map[_y][_x].build;
			build->hp -= _damages;
		}
		//if is a tower, inflict damage to it
		else if (_gd->map[_y][_x].type >= WALL)
		{
			Tower* tower = (Tower*)_gd->map[_y][_x].build;
			tower->hp -= _damages;
			//if the tower hp <= 0 : delete it
			if (tower->hp <= 0)
			{
				if (_gd->map[_y][_x].type == WALL)
				{
					FreeBlockBuild(_x, _y, _gd);
				}
				else
				{
					DeleteTower(_gd->map[_y][_x].build, _gd);
				}
				SetBlock(_x, _y, GRASS, NULL, _gd);
			}
		}

		//if the player is on the block, inflict damage;
		if ((int)_gd->player.x == _x && (int)_gd->player.y == _y)
		{
			_gd->player.hp -= _damages;
		}
	}
}

//free a block (made in case of multiples free)
void FreeBlockBuild(int _x, int _y, GameData* _gd)
{
	free(_gd->map[_y][_x].build);
}
