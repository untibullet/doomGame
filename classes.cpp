#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include <SFML/System/Vector2.hpp>

#include "func.h"
#include "classes.h"

using namespace objects;

using sf::RenderWindow;
using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using std::string;

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

	// drawSprites(win);

	// drawInterface(win);
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
	Player player = Game::player;
	float maxAngle = player.centralAngle + player.rfov / 2;

	// int countOfWallsAround = Game::player.countOfWallsAround;
	
	for (short ray = 0; ray < COUNT_OF_RAYS; ray++) {
		float angle = maxAngle - Game::deltaAngle * ray;
		float _cos = cos(angle), _sin = sin(angle);
		Vector2f direction = Vector2f(_cos, _sin);

		Wall wall = Game::level.walls[0];
		float distance = Game::renderingRadius;
		Vector2f crossingPoint;
		for (int nwall = 0; nwall < player.countOfWallsAround; nwall++) {
			wall = Game::player.wallsAround[nwall];

			bool isCrossing = false;
			crossingPoint = checkCrossing(player.position, player.centralAngle, wall.leftPos, wall.rightPos, isCrossing);

			if (!isCrossing) continue;

			distance = getDistance(player.position, crossingPoint);
			
		}
	}
}

Vector2f checkCrossing(Vector2f startPos, float angle, Vector2f p1, Vector2f p2, bool& isCrossing) {
	Vector2f crossingPoint;

	float xs = startPos.x, ys = startPos.y;
	float x1 = p1.x, y1 = p1.y;
	float x2 = p2.x, y2 = p2.y;

	float k1 = tan(angle);
	float k2 = (y2 - y1) / (x2 - x1);

	if (fabs(k1 - k2) < MIN_DEVIATION) {
		isCrossing = false;
		return crossingPoint;
	}

	float x = (k1 * xs - k2 * x1 + y1 - ys) / (k1 - k2);
	float y = k1 * (x - xs) + ys;

	bool conditionX = x1 <= x <= x2;
	bool conditionY = y1 <= y <= y2;
	if (conditionX && conditionY) {
		isCrossing = true;
		crossingPoint = Vector2f(x, y);
		return crossingPoint;
	}
	
	isCrossing = false;
	return crossingPoint;
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

Wall::Wall(Vector2f leftPos, Vector2f rightPos) {
	Wall::leftPos = leftPos;
	Wall::rightPos = rightPos;
	Wall::length = getDistance(leftPos, rightPos);
}