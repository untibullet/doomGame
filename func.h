#pragma once

#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

Vector2f multiplyVectorNumber(Vector2f v, float number);

float getDistance(sf::Vector2f p1, sf::Vector2f p2);

float getModuleOfVector(Vector2f v);

Vector2f getNormalVector(Vector2f v);

float getAngleBetweenVectors(Vector2f v1, Vector2f v2);