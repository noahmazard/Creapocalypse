#ifndef tower__H
#define tower__H
#include "definitions.h"

int InsertTower(Tower** _tree, Tower* _item);
void DisplayTowers(Tower* _tree, GameData* _gd);
void DisplayTowersLP(Tower* _tree, GameData* _gd);
void DeleteTower(Tower*, GameData*);
void PrintfTowers(Tower* _tree, GameData* _gd, int depht);
void FreeTowers(Tower* _tree, GameData* _gd);
void UpdtateTowers(Tower* tower, GameData* _gd);

void InsertShot(Shot* _shot, GameData* _gd);
void UpdateShot(GameData* _gd);
void DisplayShot(GameData* _gd);

int GetTowerPrice(int _type);
#endif
