#include "myMath.h"
#include "math.h"

float GetVectorSquaredLenght(sfVector2f _v)
{
	float lenght = (_v.x * _v.x) + (_v.y * _v.y);
	return lenght;
}
sfVector2f NormalizeVector(sfVector2f _vector)
{
	float norm = sqrtf(GetVectorSquaredLenght(_vector));
	if (norm)
	{
		_vector.x /= norm;
		_vector.y /= norm;
	}
	return _vector;
}
sfVector2f GetVector(sfVector2f _pos1, sfVector2f _pos2)
{
	sfVector2f vector =
	{
		_pos2.x - _pos1.x,
		_pos2.y - _pos1.y
	};
	
	return vector;
}