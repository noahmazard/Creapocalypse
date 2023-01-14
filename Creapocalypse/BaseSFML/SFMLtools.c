#include "SFMLtools.h"
#include "math.h"

sfSprite* LoadSprite(char* _sNom, int _isCentered)
{
	sfSprite* sprite = sfSprite_create();
	sfTexture* texture = sfTexture_createFromFile(_sNom, NULL);
	sfSprite_setTexture(sprite, texture, NULL);

	if (_isCentered) // set Origin to Center
	{
		sfVector2u size = sfTexture_getSize(texture);
		sfVector2f posCenter = { size.x / 2, size.y / 2 };
		sfSprite_setOrigin(sprite, posCenter);
	}

	return sprite;
}

sfSprite* LoadSpriteFromTexture(sfTexture* texture, int _isCentered)
{
	sfSprite* sprite = sfSprite_create();
	sfSprite_setTexture(sprite, texture, NULL);

	if (_isCentered) // set Origin to Center
	{
		sfVector2u size = sfTexture_getSize(texture);
		sfVector2f posCenter = { size.x / 2, size.y / 2 };
		sfSprite_setOrigin(sprite, posCenter);
	}

	return sprite;
}

void BlitSprite(sfSprite* _sSprite, int _x, int _y, float _angle, sfRenderWindow* _window)
{
	sfVector2f pos = { _x, _y };
	sfSprite_setPosition(_sSprite, pos);
	sfSprite_setRotation(_sSprite, _angle);
	sfRenderWindow_drawSprite(_window, _sSprite, NULL);
}

sfText* LoadText(sfFont* _font, int _size)
{
	sfText* text = sfText_create();
	sfText_setFont(text, _font);
	sfText_setCharacterSize(text, _size);
	return text;
}

void BlitText(sfText* _sText, int _x, int _y, int _size, sfRenderWindow* _window)
{
	sfVector2f pos = { _x, _y };
	sfText_setPosition(_sText, pos);
	sfText_setCharacterSize(_sText, _size);
	sfRenderWindow_drawText(_window, _sText, NULL);
}

void DrawPixel(sfImage* _sImage, int _x, int _y, sfColor _sColor)
{
	sfVector2u size = sfImage_getSize(_sImage);
	if (_x >= 0 && _y >= 0 && _x < size.x && _y < size.y)
	{
		sfImage_setPixel(_sImage, _x, _y, _sColor);
	}
}

void DrawLine(int _x1, int _y1, int _x2, int _y2, sfColor _color, sfImage* _image)
{
	if (_x1 == _x2)
	{
		if (_y1 > _y2) //On doit prendre en compte le sens pour aller de A à B afin que les boucles for marchent
		{
			int temp = _y1;
			_y1 = _y2;
			_y2 = temp;
		}
		for (int i = _y1; i <= _y2; i++)
		{
			DrawPixel(_image, _x1, i, _color);
		}
		return;
	}

	if (abs(_x2 - _x1) > abs(_y2 - _y1)) //Plutôt horizontale
	{
		if (_x1 > _x2)
		{
			DrawLine(_x2, _y2, _x1, _y1, _color, _image);
		}
		if (_y1 <= _y2) //Descendante
		{
			int dx, dy;
			int erreur;
			erreur = _x2 - _x1;
			dx = erreur * 2;
			dy = (_y2 - _y1) * 2;

			while (_x1 <= _x2)
			{
				DrawPixel(_image, _x1, _y1, _color);

				_x1++;
				erreur = erreur - dy;
				if (erreur <= 0)
				{
					_y1++;
					erreur += dx;
				}
			}
		}
		else //Montante
		{
			int dx, dy;
			int erreur;
			erreur = _x2 - _x1;
			dx = erreur * 2;
			dy = (_y1 - _y2) * 2;

			while (_x1 <= _x2)
			{
				DrawPixel(_image, _x1, _y1, _color);
				_x1++;
				erreur = erreur - dy;
				if (erreur <= 0)
				{
					_y1--;
					erreur += dx;
				}
			}
		}

	}
	else //Plutôt verticale
	{
		if (_y1 > _y2)
		{
			DrawLine(_x2, _y2, _x1, _y1, _color, _image);
		}
		if (_x1 <= _x2) //Vers la droite
		{
			int dx, dy;
			int erreur;
			erreur = _y2 - _y1;
			dy = erreur * 2;
			dx = (_x2 - _x1) * 2;

			while (_y1 <= _y2)
			{
				DrawPixel(_image, _x1, _y1, _color);
				_y1++;
				erreur = erreur - dx;
				if (erreur <= 0)
				{
					_x1++;
					erreur += dy;
				}
			}
		}
		else //Vers la gauche
		{
			int dx, dy;
			int erreur;
			erreur = _y2 - _y1;
			dy = erreur * 2;
			dx = (_x1 - _x2) * 2;

			while (_y1 <= _y2)
			{
				DrawPixel(_image, _x1, _y1, _color);
				_y1++;
				erreur = erreur - dx;
				if (erreur <= 0)
				{
					_x1--;
					erreur += dy;
				}
			}
		}
	}
}