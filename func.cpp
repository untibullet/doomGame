#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

template <typename T>
bool comp(T s1, T s2) {
	return s1.lastDistanceToPlayer < s2.lastDistanceToPlayer;
}

template <typename T>
void sorting(T* arr, short count) {
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count - (i + 1); j++) {
			if (arr[j].lastDistanceToPlayer < arr[j + 1].lastDistanceToPlayer) {
				int b = arr[j]; // создали дополнительную переменную
				arr[j] = arr[j + 1]; // меняем местами
				arr[j + 1] = b; // значения элементов
			}
		}
	}
}

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