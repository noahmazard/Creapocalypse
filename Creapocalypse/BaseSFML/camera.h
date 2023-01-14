#ifndef camera__H
#define camera__H
#include "definitions.h"

sfVector2i WorldToScreen(float _x, float _y, sfVector2f _camera);
sfVector2f ScreenToWorld(int _x, int _y, sfVector2f _camera);

#endif
