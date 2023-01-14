#include "init.h"
#include "building.h"
#include "core.h"
#include "tower.h"
#include "monster.h"

void InitWindow(GameData* _gd)
{
	//Init Window
	sfVideoMode mode = { WINDOW_WIDTH, WINDOW_HEIGHT, 32 };

#ifdef FULLSCREEN
	_gd->window = sfRenderWindow_create(mode, "SFML window", sfClose | sfFullscreen, NULL);
	sfRenderWindow_setVerticalSyncEnabled(_gd->window, sfTrue);
#else
	_gd->window = sfRenderWindow_create(mode, "SFML window", sfClose, NULL);
#endif // FULLSCREEN

	sfRenderWindow_setFramerateLimit(_gd->window, MAXFPS);
	

}

void InitSprites(GameData* _gd)
{
	sfTexture* tempTexture;
	sfTexture* tempTexture2;

	//Blocks
	tempTexture = sfTexture_createFromFile("../Sources/Grass.png", NULL);
	_gd->sprite = malloc(sizeof(sfSprite*) * NB_TILE_SPRITE);
	for (int i = 0; i < 5; i++)
	{
		_gd->sprite[i] = LoadSpriteFromTexture(tempTexture, 0);
		sfSprite_setScale(_gd->sprite[i], (sfVector2f) { 2, 2 });
		sfSprite_setColor(_gd->sprite[i], sfColor_fromRGB(50 + 15 * i, 230 + 5 * i, 50 + 15 * i));
	}
	_gd->sprite[WALL] = LoadSprite("../Sources/Wall.png", 0);
	_gd->sprite[PREVIEW] = LoadSprite("../Sources/Wall.png", 0);
	_gd->sprite[BUILDING] = _gd->sprite[WALL];
	sfSprite_setScale(_gd->sprite[WALL], (sfVector2f) { 2, 2 });
	_gd->sprite[TURRET1] = LoadSprite("../Sources/Turrets/1.png", 0);
	_gd->sprite[TURRET2] = LoadSprite("../Sources/Turrets/2.png", 0);
	_gd->sprite[TURRET3] = LoadSprite("../Sources/Turrets/3.png", 0);
	_gd->sprite[TURRET4] = LoadSprite("../Sources/Turrets/4.png", 0);

	sfSprite_setColor(_gd->sprite[PREVIEW], sfColor_fromRGBA(0, 0, 0, 0));

	for (int i = TURRET1; i <= TURRET4; i++)
	{
		sfSprite_setOrigin(_gd->sprite[i], (sfVector2f) { 0, sfTexture_getSize(sfSprite_getTexture(_gd->sprite[i])).y - 32 });
	}

	//HUD
	_gd->hudImage = sfImage_create(WINDOW_WIDTH, WINDOW_HEIGHT);
	_gd->hud = sfSprite_create();
	tempTexture2 = sfTexture_createFromImage(_gd->hudImage, NULL);
	sfSprite_setTexture(_gd->hud, tempTexture2, 0);

	//Monster
	_gd->monsterSprite[SHOOTING_MONSTER] = LoadSprite("../Sources/ignoble.png", 1);
	_gd->monsterSprite[EXPLOSIVE_MONSTER] = LoadSprite("../Sources/furoncle.png", 1);

	//Player
	_gd->player.sprite = LoadSprite("../Sources/Playerfront.bmp", 1);

	//lifeBar
	_gd->lifebar = LoadSprite("../Sources/Lifebar.png", 1);

	//balls
	_gd->ball[0] = LoadSprite("../Sources/Ball1.png", 1);
	_gd->ball[1] = LoadSprite("../Sources/Ball2.png", 1);
	_gd->ball[2] = LoadSprite("../Sources/Ball3.png", 1);
	_gd->ball[3] = LoadSprite("../Sources/Ball4.png", 1);
}

void InitGameData(GameData* _gd)
{
	//Create Map
	_gd->map = malloc(sizeof(Block*) * MAP_WIDTH);
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		_gd->map[x] = malloc(sizeof(Block) * MAP_HEIGHT);
	}

	InitWindow(_gd);
	InitSprites(_gd);

	//Init player
	
	_gd->player.actions = NULL;

	//Init Clock
	_gd->clock = sfClock_create();
	_gd->dt = 0;

	//Init Monster
	_gd->monster = NULL;

	//Init towers
	_gd->tower = NULL;
	_gd->shot = NULL;
	_gd->monsterShot = NULL;

	//Init Text
	_gd->font = sfFont_createFromFile("../Sources/font.ttf");
	_gd->text = LoadText(_gd->font, 30);

	LoadGameData(_gd);

}

void LoadGameData(GameData* _gd)
{
	//Load Stats
	_gd->stat.kill = 0;
	_gd->stat.money = 0;
	_gd->stat.towers = 0;
	strcpy(_gd->stat.name, "");
	_gd->isTyping = 1;

	//Load Highscores
	_gd->highScores = NULL;
	_gd->nbhighScores = 0;

	//Load wave
	_gd->lastWave = 120;
	_gd->wave = 0;

	//Load player properties
	_gd->player.hp = PLAYER_HP;
	_gd->player.x = MAP_WIDTH / 2;
	_gd->player.y = MAP_HEIGHT / 2;
	_gd->money = 500;

	//Init Camera
	_gd->cameraMode = CAMERA_FREE;
	_gd->camera.x = _gd->player.x * TILE_SIZE - WINDOW_WIDTH / 2;
	_gd->camera.y = _gd->player.y * TILE_SIZE - WINDOW_HEIGHT / 2;

	//Load default values
	_gd->toBuild = WALL;
	_gd->debug = 0;

	//Fill Map
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			SetBlock(x, y, rand() % 5, NULL, _gd);
		}
	}

	//Load Buildings
	LoadBuilding("Garage", _gd, 0);
	LoadBuilding("HayShed", _gd, 1);
	LoadBuilding("House", _gd, 2);
	LoadBuilding("Piggery", _gd, 3);
	LoadBuilding("Stable", _gd, 4);
}

void UnloadGameData(GameData* _gd)
{
	//Uload buils
	for (int i = 0; i < 5; i++)
	{
		sfSprite_destroy(_gd->builds[i].sprite);
	}


	//Unload monsters
	Monster* monster = _gd->monster;
	while (monster)
	{
		Monster* next = monster->next;
		KillMonster(_gd, monster);
		monster = next;
	}

	//Unload monster shots
	Shot* shot = _gd->monsterShot;
	while (shot)
	{
		Shot* next = shot->next;
		RemoveMonsterShot(shot, _gd);
		shot = next;
	}

	//Unload Highscores
	free(_gd->highScores);
	_gd->nbhighScores = 0;
}