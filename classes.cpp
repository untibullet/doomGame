#include <SFML/System/Vector2.hpp>

#include "classes.h"

void Player::set_default(sf::Vector2f position, float angle, short fov) {
	Player::position = position;
	Player::angle = angle;
	Player::fov = fov;
}