#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include <SFML/System/Vector2.hpp>

#include "func.h"

using sf::Vector2f;

float getDistance(Vector2f p1, Vector2f p2) {
	Vector2f tmp = p2 - p1;
	return sqrt(pow(tmp.x, 2) + pow(tmp.y, 2));

}