#ifndef definitions__H
#define definitions__H

#include <stdio.h>
#include <tchar.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <SFML/System.h>
#include <SFML/Network.h>
#include <SFML/Audio.h>
#include "time.h"
#include "windows.h"
#include "math.h"
#include "SFMLtools.h"

#define MAP_WIDTH 250
#define MAP_HEIGHT 250

#define TILE_SIZE 32
#define SPEED 5
#define SPEED_C 4

//comment to disable fullscreen 1080p
#define FULLSCREEN

#ifndef FULLSCREEN // =============
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#else
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#endif // !FULLSCREEN =============


#define DTMAX 0.05f //10 FPS min
#define MAXFPS 75 //75 FPS max (set it to your monitor framerate)

#define NB_BUILDS 5

#define DMG_EXPLO 300
#define HP_WALL 400
#define HP_TOWER1 500
#define HP_TOWER2 600
#define HP_TOWER3 700
#define HP_TOWER4 800

#define PRICE_WALL 5
#define PRICE_TOWER1 20
#define PRICE_TOWER2 35
#define PRICE_TOWER3 50
#define PRICE_TOWER4 100

#define PLAYER_HP 2000
#define BETWIN_WAVE 60

#define NAME_MAX_LEN 16

enum Tile
{
	GRASS = 0,
	PREVIEW = 5, // >= Preview, unwakable for player pathfinding
	BUILDING, // >= Building, unwalkable
	WALL, // >= Wall : Turret
	TURRET1,
	TURRET2,
	TURRET3,
	TURRET4,

	NB_TILE_SPRITE
}; 
enum CameraMode
{
	CAMERA_PLAYER,
	CAMERA_FREE
};
enum ActionType
{
	MOVE,
	BUILD
};
enum MonsterType
{
	EXPLOSIVE_MONSTER,
	SHOOTING_MONSTER,

	NB_MONSTER_TYPES
};

typedef struct GameData GameData;

//function pointers
typedef void (*ActionFunct)(void*, GameData*);
typedef void (*MoveFunct)(GameData*, struct Monster*);

//structure declarations

typedef struct
{
	char type;
	sfVector2f BuildPos;
}BuildData;
typedef struct PathNode
{
	sfVector2f pos;
	struct PathNode* next;
}PathNode;
typedef struct Action
{
	void* data;
	ActionFunct function;
	sfVector2f pos;
	enum ActionType type;
	struct Action* next;
}Action;
typedef struct
{
	float x;
	float y;
	sfSprite* sprite;
	Action* actions;
	int hp;
}Player;
typedef struct Monster
{
	sfVector2f pos;
	sfSprite* sprite;
	struct Monster* next;
	int hp;

	MoveFunct move;
	MoveFunct shoot;
	MoveFunct drop;
	float time;
}Monster;
typedef struct
{
	int x;
	int y;
	int hp;
	int hpMax;
	sfVector2i size;
	sfSprite* sprite;
}Buildings;
typedef struct Tower
{
	int hp;
	int hpMax;
	char type;
	int x;
	int y;
	char isPreview;
	float lastShot;
	float cd;

	//Binary tree
	struct Tower* previous;
	struct Tower* up;
	struct Tower* down;
}Tower;
typedef struct Shot
{
	int damages;
	sfVector2f pos;
	sfVector2f dir;
	float range;
	float parcoured;
	float impactRange;
	struct Shot* next;
	char isAOE;
	int type;
}Shot;
typedef struct
{
	enum Tile type;
	void* build;
}Block;
typedef struct
{
	char name[NAME_MAX_LEN];
	int kill;
	int money;
	int towers;
	time_t time;
}Statistics;
struct GameData
{
	sfSprite** sprite;
	sfRenderWindow* window;
	sfVector2f camera;
	Block** map;
	Player player;
	float dt;
	sfClock* clock;
	sfImage* hudImage;
	sfSprite* hud;
	char debug;
	Buildings builds[NB_BUILDS];
	enum CameraMode cameraMode;
	sfSprite* monsterSprite[NB_MONSTER_TYPES];
	Monster* monster;
	sfSprite* lifebar;
	Tower* tower;
	enum Tile toBuild;
	Shot* shot;
	Shot* monsterShot;
	sfSprite* ball[4];
	float lastWave;
	int wave;
	int money;
	sfText* text;
	sfFont* font;
	Statistics stat;
	Statistics* highScores;
	int nbhighScores;
	char isTyping;
};

#endif
