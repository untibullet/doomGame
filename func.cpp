#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

float getDistance(Vector2f p1, Vector2f p2) {
	Vector2f tmp = p2 - p1;
	return sqrt(pow(tmp.x, 2) + pow(tmp.y, 2));

}