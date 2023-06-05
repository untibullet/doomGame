#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include <SFML/System/Vector2.hpp>

#include "classes.h"

using namespace objects;

using sf::RenderWindow;
using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using std::string;

const short FOV = 90;
const short COUNT_OF_RAYS = 180;
const short NORMAL_DISTANCE = 4;
const float NORMAL_WALL_SIZE = 0.3;

const sf::Color GROUNG_COLOR = sf::Color(100, 100, 100);
const sf::Color SKYBOX_COLOR = sf::Color(35, 87, 97);
const sf::Color WALL_COLOR = sf::Color::Blue;

Vector2f defaultPlayerPos = Vector2f(8.f, 3.f);
float defaultPlayerFAngle = M_PI / 2;


Game::Game() {
	Game::player.set_default(defaultPlayerPos, defaultPlayerFAngle, FOV);
	Game::state.set_type("InGameplay");
	Game::deltaAngle = (float) FOV / COUNT_OF_RAYS;
}

void Game::render(RenderWindow& win) {
	drawBackGround(win);

	drawWalls(win);

	drawSprites(win);

	drawInterface(win);
}

void Game::drawBackGround(RenderWindow& win) {
	Vector2u winSize = win.getSize();

	sf::RectangleShape ground = sf::RectangleShape(Vector2f(winSize.x, winSize.y / 2));
	ground.setFillColor(GROUNG_COLOR);
	ground.setPosition(0, winSize.y / 2);
	win.draw(ground);

	sf::RectangleShape skybox = sf::RectangleShape(Vector2f(winSize.x, winSize.y / 2));
	skybox.setFillColor(SKYBOX_COLOR);
	skybox.setPosition(0, 0);
	win.draw(skybox);
}

void Game::drawWalls(RenderWindow& win) {
	float maxAngle = Game::player.centralAngle + Game::player.rfov / 2;
	
	for (short i = 0; i < COUNT_OF_RAYS; i++) {
		float angle = maxAngle - Game::deltaAngle * i;
		Vector2f direction = Vector2f(cos(angle), sin(angle));

		if (direction.x == 0 or direction.y == 0)
			continue;

		
	}
}

void Game::update() {

}

void Player::set_default(Vector2f position, float angle, short fov) {
	Player::position = position;
	Player::centralAngle = angle;
	Player::direction = Vector2f(cos(angle), sin(angle));
	Player::fov = fov;
	Player::rfov = (fov * M_PI) / 180;
}

void State::set_type(string name) {
	State::name = name;
}

Wall::Wall(Vector2f position, Vector2f size) {
	Wall::position = position;
	Wall::size = size;
}