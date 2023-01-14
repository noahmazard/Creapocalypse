#ifndef core__H
#define core__H
#include "definitions.h"

sfVector2f GetPathCollision(sfVector2f startPos, sfVector2f aimedPos, GameData*);
char IsBlockWalkable(int x, int y, char _isPreviewWalkable,char canOutOfMap, GameData*);;
void SetBlock(int _x, int _y, char _type, void* _data, GameData* _gd);
void SetBlockPreview(int _x, int _y, char _type, GameData* _gd);
void DamageBlock(int _x, int _y, int damages, GameData* _gd);
void FreeBlockBuild(int _x, int _y, GameData* _gd);

#endif
