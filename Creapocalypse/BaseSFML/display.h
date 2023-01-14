#ifndef display__H
#define display__H
#include "definitions.h"

typedef struct
{
	int minX;
	int minY;
	int maxX;
	int maxY;
}Borders;

void Display(GameData* _gd);
void DisplayLifeBar(int _current, int _max, float _x, float _y, GameData* _gd);

#endif
