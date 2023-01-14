#include "camera.h"

//Convert World position to screen position and inverse
sfVector2i WorldToScreen(float _x, float _y, sfVector2f _camera)
{
	sfVector2i ScreenPos;
	ScreenPos.x = TILE_SIZE * _x - _camera.x;
	ScreenPos.y = TILE_SIZE * _y - _camera.y;
	return ScreenPos;
}
sfVector2f ScreenToWorld(int _x, int _y, sfVector2f _camera)
{
	sfVector2f WorldPos;
	WorldPos.x = (_x + _camera.x) / TILE_SIZE;
	WorldPos.y = (_y + _camera.y) / TILE_SIZE;
	return WorldPos;
}
