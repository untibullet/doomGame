#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

Vector2f multiplyVectorNumber(Vector2f v, float number) {
	return Vector2f(v.x * number, v.y * number);
}

float getDistance(Vector2f p1, Vector2f p2) {
	Vector2f tmp = p2 - p1;
	return getModuleOfVector(tmp);
}

float getModuleOfVector(Vector2f v) {
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

Vector2f getNormalVector(Vector2f v) {
	float x, y;
	if (v.x == 0) {
		x = v.y;
		y = 0;
	}
	else {
		x = -1 * v.y;
		y = v.x;
	}

	return Vector2f(x, y);
}

float getAngleBetweenVectors(Vector2f v1, Vector2f v2) {
	float cos_ = (v1.x * v2.x + v1.y * v2.y) / 
		(getModuleOfVector(v1) * getModuleOfVector(v2));
	return acos(fabs(cos_));
}