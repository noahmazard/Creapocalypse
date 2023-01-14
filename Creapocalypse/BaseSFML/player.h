#ifndef player__H
#define player__H
#include "definitions.h"

void UpdatePlayer(GameData* _gd);

void AddPlayerAction(Player* _player, Action* _action);
void FinishPlayerAction(Player* _player);
void ClearPlayerActions(Player* _player, GameData*);

Action* CreateMoveAction(sfVector2f _pos, GameData* _gd);
Action* CreateBuildAction(sfVector2f _pos, int _type, GameData* _gd);

void Build(void* _voidData, GameData* _gd);

int A_Star(sfVector2f _start, sfVector2f _end, GameData* _gd);
#endif
