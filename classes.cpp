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

const Vector2f WALLS[14] = {Vector2f(0.f, 3.f), Vector2f(4.f, 2.f), 
Vector2f(4.f, 2.f), Vector2f(4.f, 6.f), Vector2f(0.f, 3.f), Vector2f(4.f, 6.f), 
Vector2f(9.f, -5.f),
Vector2f(9.f, 8.f), Vector2f(9.f, -5.f), Vector2f(-4.f, -5.f) };


Game::Game(RenderWindow& win) {
	Game::winSize = win.getSize();

	Game::stepInPixels = Game::winSize.x / COUNT_OF_RAYS;
	Game::wallSize = Game::winSize.y * NORMAL_WALL_SIZE;

	Game::player.set_default(defaultPlayerPos, defaultPlayerFAngle, FOV);
	Game::state.set_type("InGameplay");
	Game::deltaAngle = Game::player.rfov / COUNT_OF_RAYS;

	Game::wallsTextures.loadFromFile("walls.png");
}

Level Game::loadLevel(std::string levelName) {
	Level level;
	level.countOfWalls = 5;
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

	// drawObjects(win);

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
		Vector2f crossingPoint;
		float distance = Game::renderingRadius;
		bool haveWall = false;

		wall = findVisibleWall(haveWall, crossingPoint, distance, angle);

		if (!haveWall || distance >= Game::renderingRadius) continue;

		drawSprite(win, wall, distance, crossingPoint, maxAngle, angle, ray);
	}
}

Wall Game::findVisibleWall(bool& haveWall, Vector2f& crossingPoint, float& distance, float angle) {
	Wall wall = Game::level.walls[0];
	Wall currentWall = Game::level.walls[0];

	bool isCrossing = false;
	float tmp;
	Vector2f point;
	for (int nwall = 0; nwall < Game::player.countOfWallsAround; nwall++) {
		currentWall = Game::level.walls[nwall]; // Game::player.wallsAround

		point = checkCrossing(Game::player.position, angle, currentWall.leftPos, currentWall.rightPos, isCrossing);

		if (!isCrossing) continue;

		if (!haveWall && isCrossing) haveWall = true;

		tmp = getDistance(Game::player.position, point);

		if (tmp < distance) {
			wall = currentWall;
			crossingPoint = point;
			distance = tmp;
		}
	}

	return wall;
}

Vector2f Game::checkCrossing(Vector2f startPos, float angle, Vector2f p1, Vector2f p2, bool& isCrossing) {
	Vector2f crossingPoint;
	isCrossing = false;

	float xs = startPos.x, ys = startPos.y;
	float x1 = p1.x, y1 = p1.y;
	float x2 = p2.x, y2 = p2.y;

	float k1 = tan(angle);
	float k2 = (y2 - y1) / (x2 - x1);

	float x;
	if (x1 == x2) x = x1;
	else x = (k1 * xs - k2 * x1 + y1 - ys) / (k1 - k2);
	
	float y;
	if (y1 == y2) y = y1;
	else y = k1 * (x - xs) + ys;

	bool conditionAngle = false;
	short tmp = angle / M_PI;
	if (tmp % 2 == 0 && y >= ys) conditionAngle = true;
	else if (tmp % 2 != 0 && y <= ys) conditionAngle = true;

	bool conditionX = std::min(x1, x2) <= x && std::max(x1, x2) >= x;
	bool conditionY = std::min(y1, y2) <=y && std::max(y1, y2) >= y;

	if (conditionX && conditionY && conditionAngle) {
		isCrossing = true;
		crossingPoint = Vector2f(x, y);
	}

	return crossingPoint;
}

template <typename T>
void Game::drawSprite(RenderWindow& win, T& object, float distance, Vector2f point,
	float maxAngle, float angle, short ray) {
	
	sf::Sprite polygon;
	createPolygon(object, polygon, point, angle);

	sf::IntRect rect = polygon.getTextureRect();

	float rectHeight = (((float) NORMAL_DISTANCE / distance) * (Game::wallSize));
	short rectY = Game::winSize.y / 2 - rectHeight / 2;

	polygon.setScale(Game::stepInPixels / rect.width, (float) rectHeight / rect.height);
	polygon.setPosition(ray * Game::stepInPixels, rectY);

	// std::cout << ray * stepInPixels << " " << width << std::endl;

	win.draw(polygon);
}

template <typename T>
void Game::createPolygon(T object, sf::Sprite& polygon, Vector2f point, float angle) {
	Vector2f partOfWall = point - object.leftPos;;

	Vector2f fromBegin = Game::player.position - object.leftPos,
		fromEnd = Game::player.position - object.rightPos;

	float alpha = getAngleBetweenVectors(fromBegin, fromEnd);

	float width;
	float countOfEnteringRays;
	if (alpha > Game::player.rfov) {
		countOfEnteringRays = COUNT_OF_RAYS;
	}
	else {
		countOfEnteringRays = (COUNT_OF_RAYS * std::min(alpha, Game::player.rfov))
			/ (Game::player.rfov);
	}
	width = std::max(96 / (countOfEnteringRays * 1), 1.f);

	float tmp = getModuleOfVector(partOfWall);
	tmp /= 1; // size of wall texture in meters
	tmp = tmp - (int)tmp;

	sf::IntRect rect(96 * tmp, 288, width, 96);
	polygon.setTexture(Game::wallsTextures);
	polygon.setTextureRect(rect);
}

void Game::update() {
	// create array: player.wallsAround
}

void Player::set_default(Vector2f position, float angle, short fov) {
	Player::position = position;
	Player::centralAngle = angle;
	Player::direction = Vector2f(cos(angle), sin(angle));
	Player::fov = fov;
	Player::rfov = (fov * M_PI) / 180;
	Player::countOfWallsAround = 7;
}

void State::set_type(string name) {
	State::name = name;
}

void Wall::set_parameters(Vector2f leftPos, Vector2f rightPos) {
	Wall::leftPos = leftPos;
	Wall::rightPos = rightPos;
	Wall::length = getDistance(leftPos, rightPos);
}