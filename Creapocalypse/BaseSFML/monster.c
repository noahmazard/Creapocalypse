#include "monster.h"
#include "myMath.h"
#include "player.h"
#include "core.h"
#include "camera.h"

//Update all monsters
void UpdateMonsters(GameData* _gd)
{
	Monster* monster = _gd->monster;
	Monster* toKill = NULL;

	//Update each monster
	while (monster)
	{
		//Move it
		monster->move(_gd, monster);

		//Check if dead
		if (monster->hp <= 0)
		{
			toKill = monster;
		}
		monster = monster->next;

		//kill it
		if (toKill)
		{
			KillMonster(_gd, toKill);
			toKill = NULL;
		}
	}

	//Update wave
	_gd->lastWave += _gd->dt;

	//if last wave is over, do a new wave
	if (_gd->lastWave >= BETWIN_WAVE)
	{
		_gd->wave++;
		_gd->lastWave = 0;

		//Spawn monsters
		//North
		for (int i = 0; i < (_gd->wave / 2); i++)
		{
			SpawnMonster(_gd, EXPLOSIVE_MONSTER, (sfVector2f) { rand() % MAP_WIDTH, -( rand() % 20) });
		}
		for (int i = 0; i < (_gd->wave * _gd->wave / 4); i++)
		{
			SpawnMonster(_gd, SHOOTING_MONSTER, (sfVector2f) { rand() % MAP_WIDTH, -(rand() % 20)});
		}
		//South
		for (int i = 0; i < (_gd->wave / 2); i++)
		{
			SpawnMonster(_gd, EXPLOSIVE_MONSTER, (sfVector2f) { rand() % MAP_WIDTH , MAP_HEIGHT + rand() % 20});
		}
		for (int i = 0; i < (_gd->wave * _gd->wave / 4); i++)
		{
			SpawnMonster(_gd, SHOOTING_MONSTER, (sfVector2f) { rand() % MAP_WIDTH, MAP_HEIGHT + rand() % 20});
		}
		//East
		for (int i = 0; i < (_gd->wave / 2); i++)
		{
			SpawnMonster(_gd, EXPLOSIVE_MONSTER, (sfVector2f) { MAP_WIDTH + rand() % 20, rand() % MAP_HEIGHT });
		}
		for (int i = 0; i < (_gd->wave * _gd->wave / 4); i++)
		{
			SpawnMonster(_gd, SHOOTING_MONSTER, (sfVector2f) { MAP_WIDTH + rand() % 20, rand() % MAP_HEIGHT });
		}
		//West
		for (int i = 0; i < (_gd->wave / 2); i++)
		{
			SpawnMonster(_gd, EXPLOSIVE_MONSTER, (sfVector2f) { -( rand() % 20), rand() % MAP_HEIGHT });
		}
		for (int i = 0; i < (_gd->wave * _gd->wave / 4); i++)
		{
			SpawnMonster(_gd, SHOOTING_MONSTER, (sfVector2f) { -(rand() % 20), rand() % MAP_HEIGHT });
		}
	}
}

void MoveCreeper(GameData* _gd, Monster* _monster)
{
	//Get Vector
	sfVector2f playerPos = { _gd->player.x, _gd->player.y };
	sfVector2f nextMove = GetVector(_monster->pos, playerPos);

	//if longer than 1 : normalize it and move
	if (fabs(nextMove.x) > 0.1 || fabs(nextMove.y) > 0.1)
	{
		nextMove = NormalizeVector(nextMove);

		_monster->pos.x += nextMove.x * SPEED_C * _gd->dt;
		//if blocked, move backward and enclanched explose
		if (!IsBlockWalkable(_monster->pos.x, _monster->pos.y, 1, 1, _gd))
		{
			_monster->pos.x -= nextMove.x * SPEED_C * _gd->dt;
			if (_monster->time < 0)
			{
				_monster->time = 0;
			}
		}

		_monster->pos.y += nextMove.y * SPEED_C * _gd->dt;
		//if blocked, move backward and enclanched explose
		if (!IsBlockWalkable(_monster->pos.x, _monster->pos.y, 1, 1, _gd))
		{
			_monster->pos.y -= nextMove.y * SPEED_C * _gd->dt;

			if (_monster->time < 0)
			{
				_monster->time = 0;
			}
		}

		//if explosion enclanched, explose after 1 sec
		if (_monster->time >= 0)
		{
			_monster->time += _gd->dt;
			if (_monster->time >= 1.f)
			{
				_monster->hp = 0;
				_monster->shoot(_gd, _monster);
			}
		}
	}
	else
	{
		//Damage to player
		_monster->hp = 0;
		_monster->shoot(_gd, _monster);
	}
}
void MoveSkeleton(GameData* _gd, Monster* _monster)
{
	//Get Vector
	sfVector2f playerPos = { _gd->player.x, _gd->player.y };
	sfVector2f nextMove = GetVector(_monster->pos, playerPos, _gd);
	sfVector2f collision = GetPathCollision(_monster->pos, playerPos, _gd);
	//if player or obstacle in the shooting line at 5 block : shoot
	if (GetVectorSquaredLenght(GetVector(_monster->pos, collision)) < 25 || GetVectorSquaredLenght(nextMove) < 25)
	{
		_monster->time += _gd->dt;
		if (_monster->time >= 1)
		{
			_monster->shoot(_gd, _monster);
			_monster->time = 0;
		}
	}
	//else if longer than 1 : normalize it and move;
	else if (fabs(nextMove.x) > 0.1 || fabs(nextMove.y) > 0.1)
	{
		nextMove = NormalizeVector(nextMove);

		_monster->pos.x += nextMove.x * SPEED_C * _gd->dt;
		if (!IsBlockWalkable(_monster->pos.x, _monster->pos.y, 1, 1, _gd))
		{
			_monster->pos.x -= nextMove.x * SPEED_C * _gd->dt;
		}

		_monster->pos.y += nextMove.y * SPEED_C * _gd->dt;
		if (!IsBlockWalkable(_monster->pos.x, _monster->pos.y, 1, 1, _gd))
		{
			_monster->pos.y -= nextMove.y * SPEED_C * _gd->dt;
		}
	}
}
void ShootCreeper(GameData* _gd, Monster* _monster)
{
	//Damage all 9 block around the monster
	sfVector2i p = { (int)_monster->pos.x, (int)_monster->pos.y };
	DamageBlock(p.x + 1, p.y + 1, DMG_EXPLO, _gd);
	DamageBlock(p.x + 1, p.y    , DMG_EXPLO, _gd);
	DamageBlock(p.x + 1, p.y - 1, DMG_EXPLO, _gd);
	DamageBlock(p.x	   , p.y + 1, DMG_EXPLO, _gd);
	DamageBlock(p.x    , p.y    , DMG_EXPLO, _gd);
	DamageBlock(p.x    , p.y - 1, DMG_EXPLO, _gd);
	DamageBlock(p.x - 1, p.y + 1, DMG_EXPLO, _gd);
	DamageBlock(p.x - 1, p.y    , DMG_EXPLO, _gd);
	DamageBlock(p.x - 1, p.y - 1, DMG_EXPLO, _gd);
}
void ShootSkeleton(GameData* _gd, Monster* _monster)
{
	//Shoot in the player direction
	sfVector2f playerPos = { _gd->player.x, _gd->player.y };
	Shot* newShot = malloc(sizeof(Shot));
	newShot->pos = _monster->pos;
	newShot->next = NULL;
	newShot->damages = 50;
	newShot->range = 10;
	newShot->parcoured = 0;
	newShot->isAOE = 0;
	newShot->impactRange = 1;
	newShot->dir = NormalizeVector(GetVector(_monster->pos, playerPos));
	InsertMonsterShot(newShot, _gd);
}
void DropCreeper(GameData* _gd, Monster* _monster)
{
	//Drop money and update stats
	_gd->money += 5;
	_gd->stat.money += 5;
	_gd->stat.kill += 1;
}
void DropSkeleton(GameData* _gd, Monster* _monster)
{
	//Drop money and update stats
	_gd->money += 10;
	_gd->stat.money += 10;
	_gd->stat.kill += 1;
}
void SpawnMonster(GameData* _gd, int _type, sfVector2f _pos)
{
	//create a new monster
	Monster* newMonster = malloc(sizeof(Monster));
	newMonster->next = NULL;
	newMonster->pos = _pos;
	newMonster->sprite = _gd->monsterSprite[_type];
	newMonster->time = -1;

	switch (_type)
	{
	case EXPLOSIVE_MONSTER:
		newMonster->move = &MoveCreeper;
		newMonster->shoot = &ShootCreeper;
		newMonster->drop = &DropCreeper;
		newMonster->hp = 50;
		break;
	case SHOOTING_MONSTER:
		newMonster->move = &MoveSkeleton;
		newMonster->shoot = &ShootSkeleton;
		newMonster->drop = &DropSkeleton;
		newMonster->hp = 200;
		break;
	}

	//add it at the end of the list
	if (_gd->monster)
	{
		Monster* last = _gd->monster;
		while (last->next)
		{
			last = last->next;
		}
		last->next = newMonster;
	}
	else
	{
		_gd->monster = newMonster;
	}
}

//if the monster is on first place, update the root of the list
void KillMonster(GameData* _gd, Monster* _monster)
{
	Monster* current = _gd->monster;
	if (_gd->monster == _monster)
	{
		_gd->monster = _monster->next;
		_monster->drop(_gd, _monster);
		free(_monster);
		return;
	}

	//kill the monster
	while (current)
	{
		if (current->next == _monster)
		{
			current->next = _monster->next;
			_monster->drop(_gd, _monster);
			free(_monster);
			return;
		}
		current = current->next;
	}
}

//for each monster, if it is closest than the selected, change the selected
Monster* GetClosestMonster(sfVector2f _pos, char _isWallCollisions, int _range, GameData* _gd)
{
	Monster* monster = _gd->monster;
	Monster* choosed = NULL;
	float minDst = _range * _range;
	
	while (monster)
	{
		float dist = GetVectorSquaredLenght(GetVector(_pos, monster->pos));
		if (dist < minDst)
		{
			if (!(_isWallCollisions && GetPathCollision(_pos, monster->pos, _gd).x >= 0))
			{
				minDst = dist;
				choosed = monster;
			}
		}

		monster = monster->next;
	}
	return choosed;
}

//check all monster and return is a monster is in the range
Monster* GetCollidedMonster(sfVector2f _pos, float _range, GameData* _gd)
{
	Monster* monster = _gd->monster;

	while (monster)
	{
		float dist = GetVectorSquaredLenght(GetVector(_pos, monster->pos));
		if (dist < _range * _range)
		{
			return monster;
		}

		monster = monster->next;
	}
	return NULL;
}

//check all monster and add all monster in the range in a list that is return
Monster** GetCollidedMonsters(sfVector2f _pos, float _range, GameData* _gd)
{
	int nb = 0;
	Monster* monster = _gd->monster;
	Monster** tab;
	
	while (monster)
	{
		float dist = GetVectorSquaredLenght(GetVector(_pos, monster->pos));
		if (dist < _range * _range)
		{
			nb++;
		}

		monster = monster->next;
	}
	
	if (nb)
	{
		monster = _gd->monster;
		tab = malloc(((size_t)nb + 1) * sizeof(Monster*));

		int i = 0;
		while (monster)
		{
			float dist = GetVectorSquaredLenght(GetVector(_pos, monster->pos));
			if (dist < _range * _range)
			{
				tab[i] = monster;
				i++;
			}
			monster = monster->next;
		}
		tab[nb] = NULL;
		return tab;
	}
	return NULL;
}

//insert the shot at the root of the list
void InsertMonsterShot(Shot* _shot, GameData* _gd)
{
	
	_shot->next = _gd->monsterShot;
	_gd->monsterShot = _shot;
}

//Remove the selected shot from the list
void RemoveMonsterShot(Shot* _shot, GameData* _gd)
{
	if (_gd->monsterShot == _shot)
	{
		_gd->monsterShot = _shot->next;
		free(_shot);
	}
	else
	{
		Shot* current = _gd->monsterShot;
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

void UpdateMonsterShot(GameData* _gd)
{
	Shot* shot = _gd->monsterShot;
	Shot* nextShot;
	//Check all shots
	while (shot)
	{
		//Update shot position
		char hasTouched = 0;
		nextShot = shot->next;
		shot->pos.x += shot->dir.x * _gd->dt * 25;
		shot->pos.y += shot->dir.y * _gd->dt * 25;
		shot->parcoured += _gd->dt * 25;
		
		//if collide with block, inflict damage and destroy shot
		if (!IsBlockWalkable(shot->pos.x, shot->pos.y, 1, 1, _gd))
		{
			DamageBlock(shot->pos.x, shot->pos.y, shot->damages, _gd);
			RemoveMonsterShot(shot, _gd);
		}
		//if collide with player, inflict damage and destroy shot
		else if (GetVectorSquaredLenght(GetVector(shot->pos, (sfVector2f) { _gd->player.x, _gd->player.y })) <= 1)
		{
			_gd->player.hp -= shot->damages;
			RemoveMonsterShot(shot, _gd);
		}
		//if out of range, destroy shot
		else if (shot->parcoured >= shot->range)
		{
			RemoveMonsterShot(shot, _gd);
		}

		shot = nextShot;
	}
}

//Display each shot on screen
void DisplayMonsterShot(GameData* _gd)
{
	Shot* shot = _gd->monsterShot;
	while (shot)
	{
		sfVector2i pos = WorldToScreen(shot->pos.x,shot->pos.y,_gd->camera);
		BlitSprite(_gd->ball[0], pos.x, pos.y, 0, _gd->window);
		shot = shot->next;
	}
}
