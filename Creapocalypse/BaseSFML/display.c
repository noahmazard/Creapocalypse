#include "display.h"
#include "camera.h"
#include "tower.h"
#include "monster.h"
#include "highScore.h"

//Compute the limit of the tiles display
Borders ComputeDisplayBorders(sfVector2f* _camera)
{
	Borders border;
	border.minX = (int)_camera->x / TILE_SIZE > 0 ? (int)_camera->x / TILE_SIZE : 0;
	border.minY = (int)_camera->y / TILE_SIZE > 0 ? (int)_camera->y / TILE_SIZE : 0;

	border.maxX = ((int)_camera->x + WINDOW_WIDTH) / TILE_SIZE + 1;
	border.maxX = border.maxX > 0 ? border.maxX : 0;
	border.maxX = border.maxX < MAP_WIDTH ? border.maxX : MAP_WIDTH;

	border.maxY = ((int)_camera->y + WINDOW_HEIGHT) / TILE_SIZE + 1;
	border.maxY = border.maxY > 0 ? border.maxY : 0;
	border.maxY = border.maxY < MAP_HEIGHT ? border.maxY : MAP_HEIGHT;
	return border;
}

//Display the previews
void DisplayPreviews(GameData* _gd)
{
	//change the color of the tiles sprites to get the preview versions 
	Action* action = _gd->player.actions;
	for (int i = WALL; i < NB_TILE_SPRITE; i++)
	{
		sfSprite_setColor(_gd->sprite[i], sfColor_fromRGBA(150, 255, 255, 150));
	}
	
	//for each future actions of the player, display the preview bloc
	while (action != NULL)
	{
		if (action->type == BUILD)
		{
			BuildData* data = (BuildData*)action->data;
			sfVector2i pos = WorldToScreen((float)((int)data->BuildPos.x), (float)((int)data->BuildPos.y), _gd->camera);
			if (data->type >= TURRET1)
			{
				BlitSprite(_gd->sprite[GRASS], pos.x, pos.y, 0, _gd->window);
			}
			else
			{
				BlitSprite(_gd->sprite[data->type], pos.x, pos.y, 0, _gd->window);
			}
		}
		action = action->next;
	}

	//Reset the sprites color
	for (int i = WALL; i < NB_TILE_SPRITE; i++)
	{
		sfSprite_setColor(_gd->sprite[i], sfWhite);
	}
	
}

//Display a lifebar (tower / building)
void DisplayLifeBar(int _current, int _max, float _x, float _y, GameData* _gd)
{
	float ratio = (float)_current / _max;
	sfVector2u size = sfTexture_getSize(sfSprite_getTexture(_gd->lifebar));
	sfIntRect rect;
	sfVector2i pos = WorldToScreen(_x, _y - 0.5f, _gd->camera);

	rect.width = size.x;
	rect.height = size.y;
	rect.left = 0;
	rect.top = 0;
	sfSprite_setTextureRect(_gd->lifebar, rect);
	sfSprite_setColor(_gd->lifebar, sfBlack);
	BlitSprite(_gd->lifebar, pos.x, pos.y, 0, _gd->window);

	rect.width = size.x * ratio;
	sfSprite_setTextureRect(_gd->lifebar, rect);
	sfSprite_setColor(_gd->lifebar, sfWhite);
	BlitSprite(_gd->lifebar, pos.x, pos.y, 0, _gd->window);
}

//Display the player's life
void DisplayPlayerLife(GameData* _gd)
{
	float ratio = (float)_gd->player.hp / PLAYER_HP;
	sfVector2u size = sfTexture_getSize(sfSprite_getTexture(_gd->lifebar));
	sfIntRect rect;
	sfVector2i pos = { 80, 25 };

	rect.width = size.x;
	rect.height = size.y;
	rect.left = 0;
	rect.top = 0;
	sfSprite_setScale(_gd->lifebar, (sfVector2f) { 5, 5 });
	sfSprite_setTextureRect(_gd->lifebar, rect);
	sfSprite_setColor(_gd->lifebar, sfBlack);
	BlitSprite(_gd->lifebar, pos.x, pos.y, 0, _gd->window);

	rect.width = size.x * ratio;
	sfSprite_setTextureRect(_gd->lifebar, rect);
	sfSprite_setColor(_gd->lifebar, sfWhite);
	BlitSprite(_gd->lifebar, pos.x, pos.y, 0, _gd->window);
	sfSprite_setScale(_gd->lifebar, (sfVector2f) { 1, 1 });
}


//Display the HUD
void DisplayHUD(GameData* _gd)
{
	char text[255];

	//Display money
	sprintf(text, "%d$", _gd->money);
	sfText_setString(_gd->text, text);
	BlitText(_gd->text, 10, 50, 30, _gd->window);

	//Display wave
	sprintf(text, "Vague %d (%.0fs)", _gd->wave - 1, (double)BETWIN_WAVE - _gd->lastWave);
	sfText_setString(_gd->text, text);
	BlitText(_gd->text, 10, 80, 30, _gd->window);

	//Display the player's life
	DisplayPlayerLife(_gd);

	//Display turret icons and price
	for (int i = TURRET1; i <= TURRET4; i++)
	{
		if (_gd->toBuild != i)
		{
			sfSprite_setColor(_gd->sprite[i], sfColor_fromRGBA(150, 255, 255, 150));
		}
		BlitSprite(_gd->sprite[i], WINDOW_WIDTH/2 + (i-10.5f) * 40, WINDOW_HEIGHT - 50, 0, _gd->window);
		sfSprite_setColor(_gd->sprite[i], sfWhite);

		switch (i)
		{
		case TURRET1: 
			sprintf(text, "%3d$", PRICE_TOWER1);
			break;
		case TURRET2:
			sprintf(text, "%3d$", PRICE_TOWER2);
			break;
		case TURRET3:
			sprintf(text, "%3d$", PRICE_TOWER3);
			break;
		case TURRET4:
			sprintf(text, "%3d$", PRICE_TOWER4);
			break;
		}

		sfText_setString(_gd->text, text);
		BlitText(_gd->text, WINDOW_WIDTH / 2 + (i - 10.5f) * 40, WINDOW_HEIGHT - 20, 15, _gd->window);

	}
	if (_gd->toBuild != WALL)
	{
		sfSprite_setColor(_gd->sprite[WALL], sfColor_fromRGBA(150, 255, 255, 150));
	}
	BlitSprite(_gd->sprite[WALL], WINDOW_WIDTH / 2 + 1.5f * 40, WINDOW_HEIGHT - 50, 0, _gd->window);
	sfSprite_setColor(_gd->sprite[WALL], sfWhite);

	sprintf(text, "%3d$", PRICE_WALL);
	sfText_setString(_gd->text, text);
	BlitText(_gd->text, WINDOW_WIDTH / 2 + 1.5f * 40, WINDOW_HEIGHT - 20, 15, _gd->window);
}

//Display all the in game content
void DisplayGame(GameData* _gd)
{
	Borders border = ComputeDisplayBorders(&(_gd->camera));

	//TILE MAP
	for (int y = border.minY; y < border.maxY; y++)
	{
		for (int x = border.minX; x < border.maxX; x++)
		{
			sfVector2i pos = WorldToScreen((float)x, (float)y, _gd->camera);
			if (_gd->map[y][x].type >= TURRET1)
			{
				BlitSprite(_gd->sprite[GRASS], pos.x, pos.y, 0, _gd->window);
			}
			else
			{
				BlitSprite(_gd->sprite[_gd->map[y][x].type], pos.x, pos.y, 0, _gd->window);
			}
		}
	}

	//PREVIEW BUILDS
	DisplayPreviews(_gd);

	//BUILDS
	for (int i = 0; i < NB_BUILDS; i++)
	{
		if (_gd->builds[i].hp > 0)
		{
			sfVector2i pos = WorldToScreen((float)_gd->builds[i].x, (float)_gd->builds[i].y, _gd->camera);
			BlitSprite(_gd->builds[i].sprite, pos.x, pos.y, 0, _gd->window);
		}
	}

	//Towers
	DisplayTowers(_gd->tower, _gd);

	//Display LifeBars
	for (int i = 0; i < NB_BUILDS; i++)
	{
		if (_gd->builds[i].hp > 0)
		{
			DisplayLifeBar(_gd->builds[i].hp, _gd->builds[i].hpMax, _gd->builds[i].x + (float)_gd->builds[i].size.x / 2, (float)_gd->builds[i].y, _gd);
		}
	}
	DisplayTowersLP(_gd->tower, _gd);

	//Monsters
	Monster* monster = _gd->monster;
	while (monster)
	{
		sfVector2i pos = WorldToScreen(monster->pos.x, monster->pos.y, _gd->camera);
		BlitSprite(monster->sprite, pos.x, pos.y, 0, _gd->window);
		monster = monster->next;
	}

	//Shots
	DisplayShot(_gd);
	DisplayMonsterShot(_gd);

	//PLAYER
	sfVector2i tempPos = WorldToScreen(_gd->player.x, _gd->player.y, _gd->camera);
	BlitSprite(_gd->player.sprite, tempPos.x, tempPos.y, 0, _gd->window);

	//DEBUG
	if (_gd->player.actions && _gd->debug)
	{
		sfRenderWindow_drawSprite(_gd->window, _gd->hud, NULL);
	}

	//HUD
	DisplayHUD(_gd);
}

//Display all of the game
void Display(GameData* _gd)
{
	sfRenderWindow_clear(_gd->window, sfBlack);
	
	if (_gd->player.hp > 0)
	{
		DisplayGame(_gd);
	}
	else
	{
		DisplayScore(_gd);
	}

	sfRenderWindow_display(_gd->window);
}
