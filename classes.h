#pragma once

#include <SFML/System/Vector2.hpp>

class Player {
public:
	void set_default(sf::Vector2f position, float angle, short fov);

	sf::Vector2f position;
	short fov;
	float angle;

};

class Wall {
	public:
		Wall(sf::Vector2f position, sf::Vector2f size){
			Wall::position = position;
			Wall::size = size;
		}

		sf::Vector2f position;
		sf::Vector2f size;
};