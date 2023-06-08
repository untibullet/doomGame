#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

float getDistance(Vector2f p1, Vector2f p2) {
	Vector2f tmp = p2 - p1;
	return getModuleOfVector(tmp);
}

float getModuleOfVector(Vector2f v) {
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

float getAngleBetweenVectors(Vector2f v1, Vector2f v2) {
	float cos_ = (v1.x * v2.x + v1.y * v2.y) / 
		(getModuleOfVector(v1) * getModuleOfVector(v2));
	return acos(fabs(cos_));
}