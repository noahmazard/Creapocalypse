#ifndef monster__H
#define monster__H
#include "definitions.h"

void SpawnMonster(GameData*, int _type, sfVector2f pos);
void MoveCreeper(GameData*, Monster*);
void MoveSkeleton(GameData*, Monster*);
void UpdateMonsters(GameData* );
void KillMonster(GameData*, Monster*);
Monster* GetClosestMonster(sfVector2f _pos, char _isWallCollisions, int _range, GameData*);
Monster* GetCollidedMonster(sfVector2f _pos, float _range, GameData*);
Monster** GetCollidedMonsters(sfVector2f _pos, float _range, GameData*);

void InsertMonsterShot(Shot*, GameData*);
void UpdateMonsterShot(GameData*);
void RemoveMonsterShot(Shot*, GameData*);
void DisplayMonsterShot(GameData*);
#endif
