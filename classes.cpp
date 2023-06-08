#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

using namespace objects;

using sf::RenderWindow;

const sf::Color GROUNG_COLOR = sf::Color(100, 100, 100);
const sf::Color SKYBOX_COLOR = sf::Color(35, 87, 97);
const sf::Color WALL_COLOR = sf::Color::Blue;

Vector2f defaultPlayerPos = Vector2f(5.f, 0.f);
float defaultPlayerFAngle = 3 * M_PI / 2;

const Vector2f WALLS[4] = {Vector2f(0.f, 3.f), Vector2f(4.f, 2.f), 
Vector2f(4.f, 2.f), Vector2f(4.f, 6.f)};


Game::Game() {
	Game::player.set_default(defaultPlayerPos, defaultPlayerFAngle, FOV);
	Game::state.set_type("InGameplay");
	Game::deltaAngle = Game::player.rfov / COUNT_OF_RAYS;
}

Level Game::loadLevel(std::string levelName) {
	Level level;
	level.countOfWalls = 2;
	for (int i = 0; i < level.countOfWalls; i++) {
		level.walls[i].set_parameters(WALLS[i * 2], WALLS[i * 2 + 1]);
	}

	return level;
}

void Game::viewController(RenderWindow& win, Mouse& mouse, float time) {
	short pos = mouse.getPosition().x;
	if (Game::centralMousePos.x == pos) {
		return;
	}

	short delta = Game::centralMousePos.x - pos;
	rotate(Game::player, win.getSize().x, delta, time);
}

template <typename T>
void Game::rotate(T& object, short width, short delta, float time) {
	float tmp = (float) delta / width;
	float k = Game::mouseSpeedCoef;

	object.centralAngle += k * tmp * (object.rfov);
	float centralAngle = object.centralAngle;
	object.direction = Vector2f(cos(centralAngle), sin(centralAngle));

	if (centralAngle > 2 * M_PI) {
		object.centralAngle = centralAngle - 2 * M_PI;
	}
	else if (centralAngle < 0) {
		object.centralAngle = centralAngle + 2 * M_PI;
	}
}

void Game::movementController(sf::Keyboard::Key key, float time) {
	Vector2f playerDir = Game::player.direction;
	float dx, dy;

	float ax = playerDir.x, ay = playerDir.y, bx, by;
	if (ax == 0) {
		bx = ay;
		by = 0;
	}
	else {
		bx = -1 * ay;
		by = ax;
	}

	if (key == Keyboard::W) {
		dx = playerDir.x;
		dy = playerDir.y;
	}
	else if (key == Keyboard::S) {
		dx = playerDir.x * -1;
		dy = playerDir.y * -1;
	}
	else if (key == Keyboard::D) {
		dx = bx * -1;
		dy = by * -1;
	}
	else if (key == Keyboard::A) {
		dx = bx;
		dy = by;
	}

	if (fabs(dx) < 0.000001) dx = 0;
	if (fabs(dy) < 0.000001) dy = 0;

	move(Game::player, dx, dy, time);
}

template <typename T>
void Game::move(T& object, float dx, float dy, float time) {
	float speed = object.speed;
	float distance = speed * time; // speed[meters / seconds], time[seconds]
	float x, y;

	x = dx * distance + object.position.x;
	y = dy * distance + object.position.y;
	object.position = Vector2f(x, y);
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
	Player& player = Game::player;
	float maxAngle = player.centralAngle + player.rfov / 2;

	Vector2u winSize = win.getSize();

	// int countOfWallsAround = Game::player.countOfWallsAround;
	
	for (short ray = 0; ray < COUNT_OF_RAYS; ray++) {
		float angle = maxAngle - Game::deltaAngle * ray;

		if (angle > 2 * M_PI) {
			angle = angle - 2 * M_PI;
		}
		else if (angle < 0) {
			angle = angle + 2 * M_PI;
		}

		Wall wall = Game::level.walls[0];
		float distance = Game::renderingRadius;
		float tmp;
		Vector2f crossingPoint;
		bool haveWall = false;
		bool isCrossing = false;
		for (int nwall = 0; nwall < player.countOfWallsAround; nwall++) {
			wall = Game::level.walls[nwall]; // Game::player.wallsAround

			crossingPoint = checkCrossing(player.position, angle, wall.leftPos, wall.rightPos, isCrossing);

			if (!haveWall && isCrossing) haveWall = true;

			if (!isCrossing) continue;

			tmp = getDistance(player.position, crossingPoint);
			if (tmp < distance) distance = tmp;
		}

		if (!haveWall) continue;

		short stepInPixels = winSize.x / COUNT_OF_RAYS;
		short rectHeight = (((float)NORMAL_DISTANCE / distance) * (winSize.y * NORMAL_WALL_SIZE));
		short rectY = winSize.y / 2 - rectHeight / 2;
		sf::RectangleShape rectShape = sf::RectangleShape(Vector2f(stepInPixels, rectHeight));
		rectShape.setFillColor(sf::Color::Blue);
		rectShape.setPosition(ray * stepInPixels, rectY);
		win.draw(rectShape);
	}
}

Vector2f Game::checkCrossing(Vector2f startPos, float angle, Vector2f p1, Vector2f p2, bool& isCrossing) {
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
	float x;
	if (x1 == x2) x = x1;
	else x = (k1 * xs - k2 * x1 + y1 - ys) / (k1 - k2);
	
	float y = k1 * (x - xs) + ys;

	bool conditionAngle = false;
	short tmp = angle / M_PI;
	if (tmp % 2 == 0 && y >= ys) conditionAngle = true;
	else if (tmp % 2 != 0 && y <= ys) conditionAngle = true;

	bool conditionX = std::min(x1, x2) <= x && std::max(x1, x2) >= x;
	bool conditionY = std::min(y1, y2) <=y && std::max(y1, y2) >= y;

	if (conditionX && conditionY && conditionAngle) {
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
	Player::countOfWallsAround = 2;
}

void State::set_type(string name) {
	State::name = name;
}

void Wall::set_parameters(Vector2f leftPos, Vector2f rightPos) {
	Wall::leftPos = leftPos;
	Wall::rightPos = rightPos;
	Wall::length = getDistance(leftPos, rightPos);
}