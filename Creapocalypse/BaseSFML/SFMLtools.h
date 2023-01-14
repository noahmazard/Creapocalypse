#ifndef SFMLtools__H
#define SFMLtools__H

#include <SFML/Graphics.h>
sfSprite* LoadSprite(char*, int isCentered);
sfSprite* LoadSpriteFromTexture(sfTexture*, int isCentered);

void BlitSprite(sfSprite*, int, int, float, sfRenderWindow*);
void DrawPixel(sfImage*, int, int, sfColor);
void DrawLine(int _x1, int _y1, int _x2, int _y2, sfColor _color, sfImage* _image);

sfText* LoadText(sfFont* _font, int _size);
void BlitText(sfText* _sText, int _x, int _y, int _size, sfRenderWindow* _window);

#endif
