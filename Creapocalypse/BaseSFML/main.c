#include "definitions.h"
#include "display.h"
#include "init.h"
#include "inputs.h"
#include "update.h"


int _tmain(int argc, _TCHAR* argv[])
{
	GameData* gd = malloc(sizeof(GameData));
	InitGameData(gd);

	while (sfRenderWindow_isOpen(gd->window))
	{
		GetInputs(gd);
		Update(gd);
		Display(gd);
	}
	return 0;
}


