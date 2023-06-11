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
float defaultPlayerFAngle = 0.001;

const Vector2f WALLS[14] = { Vector2f(0.f, 3.f), Vector2f(4.f, 2.f),
Vector2f(4.f, 2.f), Vector2f(4.f, 6.f), Vector2f(0.f, 3.f), Vector2f(4.f, 6.f),
Vector2f(9.f, -5.f),
Vector2f(9.f, 8.f), Vector2f(9.f, -5.f), Vector2f(-4.f, -5.f),
Vector2f(9, 8), Vector2f(-4, 8), Vector2f(-4, 8),Vector2f(-4, -5) };


template <typename T>
void sorting(T* arr, short count) {
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count - (i + 1); j++) {
			if (arr[j].lastDistanceToPlayer < arr[j + 1].lastDistanceToPlayer) {
				T b = arr[j]; // создали дополнительную переменную
				arr[j] = arr[j + 1]; // меняем местами
				arr[j + 1] = b; // значения элементов
			}
		}
	}
}


Game::Game(RenderWindow& win) {
	Game::winSize = win.getSize();

	Game::stepInPixels = (float)Game::winSize.x / COUNT_OF_RAYS;
	Game::wallSize = Game::winSize.y * NORMAL_WALL_SIZE;

	Game::level = loadLevel(Game::levelPath);

	Game::player.set_default(defaultPlayerFAngle, FOV);
	Game::state.set_type("InGameplay");
	Game::deltaAngle = Game::player.rfov / COUNT_OF_RAYS;

	Game::wallsTextures.loadFromFile("assets/walls.png");
	Game::spritesTextures.loadFromFile("assets/things.png");

	Game::texturePack[0].loadFromFile("assets/walls.png");
	Game::texturePack[1].loadFromFile("assets/things.png");
	Game::texturePack[2].loadFromFile("assets/enemy.png");
}

Level Game::loadLevel(std::string levelPath) {
	Level level;

	std::ifstream map(levelPath);
	if (!map.is_open()) {
		std::cout << "<Not found a level>" << std::endl;
		return level;
	}

	Vector2f plPos;
	map >> plPos.x >> plPos.y;
	Game::player.position = plPos;

	int count;
	map >> count; // walls
	for (int i = 0; i < count; i++) {
		float x1, y1, x2, y2;
		int mod;

		map >> x1;

		if (x1 == -9999) {
			count = i;
			break;
		}

		map >> y1 >> x2 >> y2 >> mod;
		level.walls[i].set_parameters(Vector2f(x1, y1), Vector2f(x2, y2), wallSize);
	}
	level.countOfWalls = count;

	map >> count; // things
	for (int i = 0; i < count; i++) {
		float x1, y1, xpick, ypick, wdth, hght, relSize;
		std::string name;

		map >> x1;

		if (x1 == -9999) {
			count = i + 1;
			break;
		}

		map >> y1 >> xpick >> ypick >> wdth >> hght >> relSize;
		level.spriteObjects[i].setParameters(x1, y1, xpick, ypick, wdth, hght, relSize, name);
	}
	level.countOfSpriteObjects = count;

	// map >> count; // enemeis

	map.close();
	
	return level;
}

void Game::createPresets() {
	// wall_0
	presets[0].setParameters(0, "wall_0", Vector2i(0, 288), Vector2i(96, 96));

	// wall_2
	presets[1].setParameters(0, "wall_1", Vector2i(384, 288), Vector2i(96, 96));

	// aidkit
	presets[2].setParameters(1, "aidkit", Vector2i(17, 65), Vector2i(14, 14));

	// enemy_0
	presets[3].setParameters(2, "enemy_0", Vector2i(483, 208), Vector2i(41, 51));
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
	float tmp = (float)delta / width;
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
	Vector2f normVec = getNormalVector(playerDir);
	bx = normVec.x, by = normVec.y;

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

		if (haveWall && distance <= Game::renderingRadius)
			drawSprite(win, wall, distance, crossingPoint, maxAngle, angle, ray);

		int count = 0;
		SpriteObject* sprites;
		sprites = findVisibleSpriteObjects(count, angle, distance);

		sorting(sprites, count);

		SpriteObject sprite;
		for (short i = 0; i < count; i++) {
			sprite = sprites[i];
			drawSprite(win, sprite, sprite.lastDistanceToPlayer, sprite.pointOfCollision,
				maxAngle, angle, ray);
		}

		// distance *= cos(-1 * Game::player.rfov / 2 + deltaAngle * ray);
	}
}

Wall Game::findVisibleWall(bool& haveWall, Vector2f& crossingPoint, float& distance, float angle) {
	Wall wall = Game::level.walls[0];
	Wall currentWall = Game::level.walls[0];

	bool isCrossing = false;
	float tmp;
	Vector2f point;
	for (int nwall = 0; nwall < Game::player.countOfWallsAround; nwall++) {
		currentWall = Game::player.wallsAround[nwall];

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

SpriteObject* Game::findVisibleSpriteObjects(int& count, float angle, float maxRenderDistance) {
	SpriteObject output[MAX_SPRITES_AROUND];

	int n = 0;
	bool isCrossing;
	Vector2f point;
	float distance;
	for (int i = 0; i < player.countOfSpriteObjectsAround; i++) {
		isCrossing = false;
		SpriteObject& obj = player.spriteObjectsAround[i];
		point = checkCrossing(player.position, angle, obj.leftPos, obj.rightPos, isCrossing);
		if (!isCrossing) continue;

		distance = getDistance(player.position, point);
		if (distance > maxRenderDistance) continue;

		obj.lastDistanceToPlayer = distance;
		obj.pointOfCollision = point;
		
		output[n] = obj;
		n++, count++;
	}

	return output;
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
	bool conditionY = std::min(y1, y2) <= y && std::max(y1, y2) >= y;

	if (conditionX && conditionY && conditionAngle) {
		isCrossing = true;
		crossingPoint = Vector2f(x, y);
	}

	return crossingPoint;
}

template <typename T>
void Game::drawSprite(RenderWindow& win, T& obj, float distance, Vector2f point,
	float maxAngle, float angle, short ray) {

	sf::Sprite polygon;
	createPolygon(obj, polygon, point, angle);

	sf::IntRect rect = polygon.getTextureRect();

	float kdis = (float)NORMAL_DISTANCE / distance;
	float rectHeight = (kdis * (obj.sizeInPixels));
	short rectY = Game::winSize.y / 2 - rectHeight / 2;

	polygon.setScale(Game::stepInPixels / rect.width, (float)rectHeight / rect.height);
	polygon.setPosition(ray * Game::stepInPixels, rectY + kdis * obj.heigthShift);

	// std::cout << ray * stepInPixels << " " << width << std::endl;

	win.draw(polygon);
}

template <typename T>
void Game::createPolygon(T obj, sf::Sprite& polygon, Vector2f point, float angle) {
	Vector2f partOfWall = point - obj.leftPos;;

	Vector2f fromBegin = Game::player.position - obj.leftPos,
		fromEnd = Game::player.position - obj.rightPos;

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
	width = std::max(obj.sizeInTexture.x / (countOfEnteringRays * 1), 1.f);

	float tmp = getModuleOfVector(partOfWall);
	tmp /= 1; // size of wall texture in meters
	if (obj.type == "wall") tmp = tmp - (int)tmp;
	else tmp /= obj.length;

	sf::IntRect rect(obj.positionInTexture.x + obj.sizeInTexture.x * tmp,
		obj.positionInTexture.y, width, obj.sizeInTexture.y);
	if (obj.type == "wall") polygon.setTexture(Game::wallsTextures);
	else polygon.setTexture(Game::spritesTextures);
	polygon.setTextureRect(rect);
}

void Game::update() {
	updateRenderingArrays();

	updateSpriteObjectsAround();
}

void Game::updateRenderingArrays() {
	int n = 0;
	for (int i = 0; i < level.countOfWalls; i++) {
		Wall& wall = level.walls[i];
		float dis[3];
		dis[0] = getDistance(wall.leftPos, player.position);
		dis[1] = getDistance(wall.middlePos, player.position);
		dis[2] = getDistance(wall.rightPos, player.position);

		if (std::min(std::min(dis[0], dis[1]), dis[2]) > renderingRadius)
			continue;

		player.wallsAround[n] = wall;
		n++;
	}
	player.countOfWallsAround = n;

	n = 0;
	for (int i = 0; i < level.countOfSpriteObjects; i++) {
		SpriteObject& obj = level.spriteObjects[i];
		float dis = getDistance(obj.middlePos, player.position);

		if (dis > renderingRadius)
			continue;

		player.spriteObjectsAround[n] = obj;
		n++;
	}
	player.countOfSpriteObjectsAround = n;
}

void Game::updateSpriteObjectsAround() {
	for (int i = 0; i < player.countOfSpriteObjectsAround; i++) {
		SpriteObject& obj = player.spriteObjectsAround[i];

		Vector2f normalVector = getNormalVector(player.position - obj.middlePos);
		float k = (obj.length / 2) / getModuleOfVector(normalVector);
		obj.rightPos = multiplyVectorNumber(normalVector, k) + obj.middlePos;
		obj.leftPos = multiplyVectorNumber(normalVector, k * -1) + obj.middlePos;
	}
}

void Player::set_default(float angle, short fov) {
	Player::position = position;
	Player::centralAngle = angle;
	Player::direction = Vector2f(cos(angle), sin(angle));
	Player::fov = fov;
	Player::rfov = (fov * M_PI) / 180;
	Player::countOfWallsAround = 4;
}

void State::set_type(string name) {
	State::name = name;
}

void SpriteObject::setParameters(float x1, float y1, short xpick, short ypick, short wdth, 
	short hght, float length, std::string name) {
	SpriteObject::type = name;
	SpriteObject::middlePos = Vector2f(x1, y1);

	SpriteObject::positionInTexture = Vector2i(xpick, ypick);
	SpriteObject::sizeInTexture = Vector2i(wdth, hght);

	SpriteObject::length = length;
}

void Wall::set_parameters(Vector2f leftPos, Vector2f rightPos, float sizeInPixels) {
	Wall::leftPos = leftPos;
	Wall::rightPos = rightPos;

	Vector2f tmp = rightPos - leftPos;
	Wall::middlePos = Vector2f(tmp.x / 2, tmp.y / 2) + leftPos;

	Wall::length = getDistance(leftPos, rightPos);

	Wall::sizeInPixels = sizeInPixels;
}

void SpritesPreset::setParameters(short type, std::string name,
	Vector2i pos, Vector2i size) {
	SpritesPreset::type = type;
	SpritesPreset::name = name;

	positionInTileMap = pos;
	sizeInTileMap = size;


}