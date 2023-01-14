#include "building.h"
#include "core.h"

//Load a building from a config file
void LoadBuilding(char* _fileName, GameData* _gd, int _id)
{
	char path[128] = "../Build/";
	char word[128];
	FILE* file;

	//Open file
	strcat(path, _fileName);
	strcat(path, ".txt");
	file = fopen(path, "rt");

	//Check error
	if (!file)
	{
		printf("Error while reading \"%s\"", path);
		perror(" ");
		exit(EXIT_FAILURE);
	}

	//Read the file
	while (fscanf(file, "%s", word) != EOF)
	{
		// #POSITION %d %d
		if (strcmp(word, "#POSITION") == 0)
		{
			fscanf(file, "%d %d", &_gd->builds[_id].x, &_gd->builds[_id].y);
		}

		// #SIZE %d %d : size x and y
		// next %d are a 2d representation of the building (0 : grass, 1 : building bloc)
		if (strcmp(word, "#SIZE") == 0)
		{
			int tile = 1;
			fscanf(file, "%d %d", &_gd->builds[_id].size.x, &_gd->builds[_id].size.y);

			for (int y = 0; y < _gd->builds[_id].size.y; y++)
			{
				for (int x = 0; x < _gd->builds[_id].size.x; x++)
				{
					fscanf(file, "%d", &tile);
					SetBlock(_gd->builds[_id].x + x, _gd->builds[_id].y + y, BUILDING * tile, &_gd->builds[_id], _gd);
				}
			}
		}

		//#SPRITE %s : name of sprite used
		if (strcmp(word, "#SPRITE") == 0)
		{
			strcpy(path, "../Sources/Builds/");
			fscanf(file, "%s", word);
			strcat(path, word);
			_gd->builds[_id].sprite = LoadSprite(path, 0);
		}

		// #OFFSET %f %f : x and y offset of the build
		if (strcmp(word, "#OFFSET") == 0)
		{
			sfVector2f offset;
			fscanf(file, "%f %f", &offset.x, &offset.y);
			sfSprite_setOrigin(_gd->builds[_id].sprite, offset);
		}

		// #HP %d : hp of the build
		if (strcmp(word, "#HP") == 0)
		{
			fscanf(file, "%d", &_gd->builds[_id].hp);
			_gd->builds[_id].hpMax = _gd->builds[_id].hp;
		}
	}
}

void UpdateBuildings(GameData* _gd)
{
	//For each building
	for (int i = 0; i < NB_BUILDS; i++)
	{
		//If destroyed
		if (_gd->builds[i].hp <= 0)
		{
			//for each block of the building
			for (int x = 0; x < _gd->builds[i].size.x; x++)
			{
				for (int y = 0; y < _gd->builds[i].size.y; y++)
				{
					//that are a building block
					if (_gd->map[_gd->builds[i].y + y][_gd->builds[i].x + x].type == BUILDING)
					{
						//Set it to GRASS
						SetBlock(_gd->builds[i].x + x, _gd->builds[i].y + y, GRASS, NULL, _gd);
					}
				}
			}
		}
	}
}

