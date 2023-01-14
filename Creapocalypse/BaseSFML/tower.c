#include "tower.h"
#include "camera.h"
#include "display.h"
#include "monster.h"
#include "myMath.h"


//The tower system work with a binary tree.
//This allow to easily sort them to display the uppers tower first.


//Insert a tower in the binary three
int InsertTower(Tower** _tree, Tower* _item)
{
	if (_item)
	{
		if (*_tree == NULL)
		{
			*_tree = _item;
			return 1;
		}
		else if (_item->y < (*_tree)->y)
		{
			if (InsertTower(&(*_tree)->up, _item))
			{
				_item->previous = *_tree;
			}
		}
		else if (_item->y > (*_tree)->y)
		{
			if (InsertTower(&(*_tree)->down, _item))
			{
				_item->previous = *_tree;
			}
		}
		else if (_item->x < (*_tree)->x)
		{
			if (InsertTower(&(*_tree)->up, _item))
			{
				_item->previous = *_tree;
			}
		}
		else
		{
			if (InsertTower(&(*_tree)->down, _item))
			{
				_item->previous = *_tree;
			}
		}
		return 0;
	}
	return 0;
}

//Display all towers on screen
void DisplayTowers(Tower* _tree, GameData* _gd)
{
	if (_tree)
	{
		if (_tree->up)
		{
			DisplayTowers(_tree->up, _gd);
		}
		sfVector2i pos = WorldToScreen(_tree->x, _tree->y, _gd->camera);
		if (_tree->isPreview)
		{
			sfSprite_setColor(_gd->sprite[_tree->type], sfColor_fromRGBA(150, 255, 255, 150));
		}
		BlitSprite(_gd->sprite[_tree->type], pos.x, pos.y, 0, _gd->window);
		if (_tree->isPreview)
		{
			sfSprite_setColor(_gd->sprite[_tree->type], sfWhite);
		}
		if (_tree->down)
		{
			DisplayTowers(_tree->down, _gd);
		}
	}
}

//Display all towers Life Bar on screen
void DisplayTowersLP(Tower* _tree, GameData* _gd)
{
	if (_tree)
	{
		if (_tree->up)
		{
			DisplayTowersLP(_tree->up, _gd);
		}

		if (!_tree->isPreview && _tree->hp < _tree->hpMax)
		{
			DisplayLifeBar(_tree->hp, _tree->hpMax, _tree->x + 0.5, _tree->y, _gd);
		}

		if (_tree->down)
		{
			DisplayTowersLP(_tree->down, _gd);
		}
	}
}

//Display the list of all tower in the terminal
void PrintfTowers(Tower* _tree, GameData* _gd, int _depht)
{
	if (_tree)
	{
		if (_tree->up)
		{
			PrintfTowers(_tree->up, _gd, _depht + 1);
		}
		
		printf("  %d\t %d %d : %d (preview ? : %d), %d/%d\n", _depht, _tree->x, _tree->y, _tree->type, _tree->isPreview, _tree->hp, _tree->hpMax);

		if (_tree->down)
		{
			PrintfTowers(_tree->down, _gd, _depht + 1);
		}
	}
}

//Free all tower of the binary three
void FreeTowers(Tower* _tree, GameData* _gd)
{
	if (_tree)
	{
		if (_tree->up)
		{
			FreeTowers(_tree->up, _gd);
		}
		
		if (_tree->down)
		{
			FreeTowers(_tree->down, _gd);
		}

		free(_tree);
	}
}

//Free a single tower
void DeleteTower(Tower* _tower, GameData* _gd)
{
	if (_tower->up != NULL && _tower->down != NULL) // down + up
	{
		_tower->up->previous = NULL;
		InsertTower(&(_tower->down), _tower->up);
	}

	if (_tower->previous)
	{
		if (_tower->down) // down
		{
			if (_tower->previous->down == _tower)
			{
				_tower->previous->down = _tower->down;
				_tower->down->previous = _tower->previous;
			}
			if (_tower->previous->up == _tower)
			{
				_tower->previous->up = _tower->down;
				_tower->down->previous = _tower->previous;
			}
		}
		else if (_tower->up) //up
		{
			if (_tower->previous->down == _tower)
			{
				_tower->previous->down = _tower->up;
				_tower->up->previous = _tower->previous;
			}
			if (_tower->previous->up == _tower)
			{
				_tower->previous->up = _tower->up;
				_tower->up->previous = _tower->previous;
			}
		}
		else
		{
			if (_tower->previous->down == _tower)
			{
				_tower->previous->down = NULL;
			}
			if (_tower->previous->up == _tower)
			{
				_tower->previous->up = NULL;
			}
		}
	}
	else //start
	{
		if (_tower->down) 
		{
			_gd->tower = _tower->down;
			_gd->tower->previous = NULL;
		}
		else if (_tower->up)
		{
			_gd->tower = _tower->up;
			_gd->tower->previous = NULL;
		}
		else
		{
			_gd->tower = NULL;
		}
	}

	free(_tower);
}

//Update all towers
void UpdtateTowers(Tower* tower, GameData* _gd)
{
	if (tower)
	{
		//increase a cooldown
		tower->lastShot += _gd->dt;

		//shoot after the cooldown
		if (tower->lastShot >= tower->cd && _gd->map[tower->y][tower->x].type >= TURRET1)
		{
			//set the parameters
			int damages = 0;
			int range = 0;
			char doesWallCollide = 0;
			float impactRange = 0;
			char isAOE = 0;

			switch (tower->type)
			{
			case TURRET1:
				damages = 10;
				range = 10;
				doesWallCollide = 1;
				impactRange = 0.5f;
				isAOE = 0;
				break;
			case TURRET2:
				damages = 15;
				range = 15;
				doesWallCollide = 1;
				impactRange = 0.5f;
				isAOE = 0;
				break;
			case TURRET3:
				damages = 25;
				range = 12;
				doesWallCollide = 1;
				impactRange = 0.5f;
				isAOE = 0;
				break;
			case TURRET4:
				damages = 50;
				range = 15;
				doesWallCollide = 0;
				impactRange = 1.5f;
				isAOE = 1;
				break;
			default:
				break;
			}

			//get the closest monster in a range, if found, create and add the shot to the linek list.
			Monster* monster = GetClosestMonster((sfVector2f) { tower->x, tower->y }, doesWallCollide, range,_gd);
			if (monster)
			{
				Shot* newShot = malloc(sizeof(Shot));
				newShot->pos = (sfVector2f){ tower->x + 0.5f ,tower->y + 0.5f };
				newShot->next = NULL;
				newShot->damages = damages;
				newShot->range = range;
				newShot->parcoured = 0;
				newShot->isAOE = isAOE;
				newShot->impactRange = impactRange;
				newShot->dir = NormalizeVector(GetVector(newShot->pos, monster->pos));
				newShot->type = tower->type;
				InsertShot(newShot, _gd);
				tower->lastShot = 0;
			}
		}

		//do it for the others tower
		if (tower->up)
		{
			UpdtateTowers(tower->up, _gd);
		}
		if (tower->down)
		{
			UpdtateTowers(tower->down, _gd);
		}
	}
}

//Return the type of turrets's price
int GetTowerPrice(int _type)
{
	int price;
	switch (_type)
	{
	default: price = 0; break;
	case WALL: price = PRICE_WALL; break;
	case TURRET1: price = PRICE_TOWER1; break;
	case TURRET2: price = PRICE_TOWER2; break;
	case TURRET3: price = PRICE_TOWER3; break;
	case TURRET4: price = PRICE_TOWER4; break;
	}
	return price;
}

//Insert shots at the linked list root
void InsertShot(Shot* _shot, GameData* _gd)
{
	_shot->next = _gd->shot;
	_gd->shot = _shot;
}

//Free a certain shot from the linked list
void RemoveShot(Shot* _shot, GameData* _gd)
{
	if (_gd->shot == _shot)
	{
		_gd->shot = _shot->next;
		free(_shot);
	}
	else
	{
		Shot* current = _gd->shot;
		while (current->next)
		{
			if (current->next == _shot)
			{
				current->next = _shot->next;
				free(_shot);
				return;
			}
			current = current->next;
		}
	}
}

//Update all shots
void UpdateShot(GameData* _gd)
{
	Shot* shot = _gd->shot;
	Shot* nextShot;
	//for each shots
	while (shot)
	{
		//update it pos
		char hasTouched = 0;
		nextShot = shot->next;
		shot->pos.x += shot->dir.x * _gd->dt * 25;
		shot->pos.y += shot->dir.y * _gd->dt * 25;
		shot->parcoured += _gd->dt * 25;
		
		//if collide with a monster, inflict damage and remove shot
		Monster* monster = GetCollidedMonster((sfVector2f) { shot->pos.x, shot->pos.y }, shot->isAOE ? 0.5f : shot->impactRange, _gd);
		if (monster)
		{
			//if the shot is AOE (missile), inflict damages to all monster in range
			if (shot->isAOE)
			{
				Monster** monsters = GetCollidedMonsters((sfVector2f) { shot->pos.x, shot->pos.y }, shot->impactRange, _gd);
				for (int i = 0; monsters[i] != NULL; i++)
				{
					monsters[i]->hp -= shot->damages;
				}
				RemoveShot(shot, _gd);
				free(monsters);
			}
			//else inflict to one of them
			else
			{
				monster->hp -= shot->damages;
				RemoveShot(shot, _gd);
			}
		}

		//if shot out of range, destroy it
		else if (shot->parcoured >= shot->range)
		{
			RemoveShot(shot, _gd);
		}

		shot = nextShot;
	}
}

//Display all shots
void DisplayShot(GameData* _gd)
{
	Shot* shot = _gd->shot;
	while (shot)
	{
		sfVector2i pos = WorldToScreen(shot->pos.x,shot->pos.y,_gd->camera);
		BlitSprite(_gd->ball[shot->type - TURRET1], pos.x, pos.y, shot->type == TURRET4 ? atan2f(shot->dir.y, shot->dir.x) * 57.2957795f : 0, _gd->window);
		shot = shot->next;
	}
}
