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
const sf::Color AMMO_TEXT_COLOR = sf::Color(191, 182, 57, 200);
const sf::Color HP_TEXT_COLOR = sf::Color(181, 9, 15, 200);
const sf::Color ARM_TEXT_COLOR = sf::Color(5, 14, 150, 200);

Vector2f defaultPlayerPos = Vector2f(-4.56137, 0.51811);
float defaultPlayerFAngle = 0.f;

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

	createPresets();

	Game::level = loadLevel(Game::levelPath);

	Game::player.set_default(defaultPlayerFAngle, FOV);
	Game::state.set_type("InGameplay");

	Game::deltaAngle = Game::player.rfov / COUNT_OF_RAYS;

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
		short _preset;

		map >> x1;

		if (x1 == -9999) {
			count = i;
			break;
		}

		map >> y1 >> x2 >> y2 >> _preset;
		level.walls[i].set_parameters(Vector2f(x1, y1), Vector2f(x2, y2), presets[_preset]);
	}
	level.countOfWalls = count;

	map >> count; // sprites
	for (int i = 0; i < count; i++) {
		float x1, y1, length;
		short _preset;

		map >> x1;

		if (x1 == -9999) {
			count = i;
			break;
		}

		map >> y1 >> length >> _preset;
		level.spriteObjects[i].setParameters(x1, y1, length, presets[_preset]);
	}
	level.countOfSpriteObjects = count;

	map.close();
	
	return level;
}

void Game::createPresets() {
	// walls
	presets[0].setParameters(0, "wall", Vector2i(0, 288), Vector2i(96, 96), 
		wallSize, 0);
	presets[1].setParameters(0, "wall", Vector2i(0, 384), Vector2i(96, 96),
		wallSize, 0);
	presets[2].setParameters(0, "wall", Vector2i(480, 96), Vector2i(96, 96),
		wallSize, 0);
	presets[3].setParameters(0, "wall", Vector2i(288, 0), Vector2i(96, 96),
		wallSize, 0);
	presets[4].setParameters(0, "wall", Vector2i(384, 480), Vector2i(96, 96),
		wallSize, 0);

	float dh = winSize.y * 0.15;
	short dst = winSize.y * 0.125;
	// aidkit
	presets[5].setParameters(1, "aidkit", Vector2i(17, 65), Vector2i(14, 14),
		dh, dst);
	presets[6].setParameters(1, "aidkit", Vector2i(3, 34), Vector2i(11, 13),
		dh, dst);
	presets[7].setParameters(1, "aidkit", Vector2i(2, 82), Vector2i(12, 12),
		dh, dst);
	// for things: dst + (dh - h)
	// armour
	presets[8].setParameters(1, "armour", Vector2i(97, 33), Vector2i(14, 14),
		dh, dst);
	presets[9].setParameters(1, "armour", Vector2i(99, 2), Vector2i(10, 12),
		dh, dst);
	// ammo
	presets[10].setParameters(1, "ammo", Vector2i(50, 99), Vector2i(11, 11),
		dh, dst);

	dh = winSize.y * 0.275;
	dst = (wallSize - dh) / 2;
	// enemy
	presets[11].setParameters(2, "enemy", Vector2i(483, 208), Vector2i(41, 51),
		dh, dst);
	presets[12].setParameters(2, "enemy", Vector2i(51, 208), Vector2i(41, 51),
		dh, dst);
	presets[13].setParameters(2, "enemy", Vector2i(195, 0), Vector2i(41, 51),
		dh, dst);
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

	if (key == sf::Keyboard::Key::W) {
		dx = playerDir.x;
		dy = playerDir.y;
	}
	else if (key == sf::Keyboard::Key::S) {
		dx = playerDir.x * -1;
		dy = playerDir.y * -1;
	}
	else if (key == sf::Keyboard::Key::D) {
		dx = bx * -1;
		dy = by * -1;
	}
	else if (key == sf::Keyboard::Key::A) {
		dx = bx;
		dy = by;
	}

	if (fabs(dx) < 0.000001) dx = 0;
	if (fabs(dy) < 0.000001) dy = 0;

	float distance = 1;
	bool haveWall = false;
	Vector2f point;
	Wall collisionWall = findVisibleWall(haveWall, point, distance, acos(dx));

	if (distance > 0.4 || !haveWall) move(Game::player, dx, dy, time);
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

void Game::shoot() {
	if (player.ammo <= 0 || player.animation->type != "hold") return;

	player.animation = &player.animations[1];
	player.ammo -= 2;

	updateRenderingArrays();
	updateSpriteObjectsAround();

	SpriteObject sprite;
	short n = 0;
	bool wasHit = false;
	for (int i = 0; i < level.countOfSpriteObjects; i++) {
		sprite = level.spriteObjects[i];

		if (sprite.preset.name != "enemy" || wasHit) {
			level.spriteObjects[n] = sprite;
			n++;
			continue;
		}

		bool isHit = false;
		checkCrossing(player.position, player.centralAngle, sprite.leftPos,
			sprite.rightPos, isHit);

		if (!isHit) {
			level.spriteObjects[n] = sprite;
			n++;
			continue;
		}

		sprite.health -= player.damage;
		wasHit = true;
		std::cout << "hit" << std::endl;

		continue;

		if (sprite.health < 0) {
			continue;
		}
		
		level.spriteObjects[n] = sprite;
		n++;
	}
	level.countOfSpriteObjects = n;
}

void Game::render(RenderWindow& win, float time) {
	drawBackGround(win);

	drawWalls(win);

	// drawObjects(win);

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
	SpritesPreset preset = obj.preset;

	float kdis = (float)NORMAL_DISTANCE / distance;
	float rectHeight = (kdis * (preset.defaultHeight));
	short rectY = Game::winSize.y / 2 - rectHeight / 2;

	polygon.setScale(Game::stepInPixels / rect.width, (float)rectHeight / rect.height);
	polygon.setPosition(ray * Game::stepInPixels, rectY + kdis * preset.heightShift);

	// std::cout << ray * stepInPixels << " " << width << std::endl;

	win.draw(polygon);
}

template <typename T>
void Game::createPolygon(T obj, sf::Sprite& polygon, Vector2f point, float angle) {
	Vector2f partOfWall = point - obj.leftPos;;

	Vector2f fromBegin = Game::player.position - obj.leftPos,
		fromEnd = Game::player.position - obj.rightPos;

	SpritesPreset preset = obj.preset;

	if (preset.type < 0 || preset.type > 50) return;

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
	width = std::max(preset.sizeInTileMap.x / (countOfEnteringRays * 1), 1.f);

	float tmp = getModuleOfVector(partOfWall);
	tmp /= 1; // size of wall texture in meters
	if (preset.name == "wall") tmp = tmp - (int)tmp;
	else tmp /= obj.length;

	sf::IntRect rect = sf::IntRect(preset.positionInTileMap.x + preset.sizeInTileMap.x * tmp,
		preset.positionInTileMap.y, width, preset.sizeInTileMap.y);

	polygon.setTexture(texturePack[preset.type]);
	polygon.setTextureRect(rect);
}

void Game::drawInterface(RenderWindow& win) {
	// drawCrosshair(win);

	drawWeapon(win);

	drawStats(win);
}

void Game::drawCrosshair(RenderWindow& win) {
	float radius = 10.f;
	sf::CircleShape crosshair(radius);

	crosshair.setFillColor(sf::Color(0, 0, 0, 0));
	crosshair.setOutlineColor(sf::Color(10, 185, 10, 200));
	crosshair.setOutlineThickness(2.f);

	crosshair.setPosition(winSize.x / 2 - radius, winSize.y / 2 - radius);

	win.draw(crosshair);
}

void Game::drawWeapon(RenderWindow& win) {
	sf::Sprite sprite;
	Animation* anim = player.animation;

	sprite.setTexture(anim->texture);

	SpritesPreset& frame = anim->frames[anim->currentFrame];
	int x = frame.positionInTileMap.x, y = frame.positionInTileMap.y;
	int w = frame.sizeInTileMap.x, h = frame.sizeInTileMap.y;
	sf::IntRect rect(x, y, w, h);

	sprite.setTextureRect(rect);

	Vector2u wsize = win.getSize();
	float a = 3, b = 2.7;
	sprite.setScale(3, 2.7);

	sprite.setPosition(wsize.x * 0.7 - a * (float)rect.width / 2, (float)wsize.y - (float)rect.height * b);

	win.draw(sprite);
}

void Game::drawStats(RenderWindow& win) {
	sf::Text stats;
	sf::Font font;
	font.loadFromFile("assets/pixel_regular.ttf");
	stats.setFont(font);
	stats.setCharacterSize(72);
	stats.setStyle(sf::Text::Regular);

	stats.setFillColor(HP_TEXT_COLOR);
	stats.setString("HP " + std::to_string(player.health));
	stats.setPosition((float)winSize.x * 0.1,
		(float)winSize.y * 0.75);
	win.draw(stats);

	stats.setFillColor(ARM_TEXT_COLOR);
	stats.setString("ARMOUR " + std::to_string(player.armour));
	stats.setPosition((float)winSize.x * 0.1,
		(float)winSize.y * 0.85);
	win.draw(stats);

	stats.setCharacterSize(52);
	stats.setFillColor(AMMO_TEXT_COLOR);
	stats.setString("AMMO " + std::to_string(player.ammo));
	stats.setPosition((float)winSize.x * 0.8,
		(float)winSize.y * 0.85);
	win.draw(stats);
}

void Game::update(float time) {
	updateRenderingArrays();

	updateSpriteObjectsAround();

	player.changeFrame(time);
	if (player.delayTakeDamage > 0) {
		player.delayTakeDamage -= time;
	}

	SpriteObject sprite;
	short n = 0;
	for (int i = 0; i < level.countOfSpriteObjects; i++) {
		sprite = level.spriteObjects[i];
		if (sprite.lastDistanceToPlayer > INTERACTION_RADIUS) {
			level.spriteObjects[n] = sprite;
			n++;
			continue;
		}

		if (sprite.preset.name == "enemy" && player.delayTakeDamage <= 0) {
			if (player.armour > 0) player.armour -= std::min(20, (int)player.armour);
			else player.health -= 20;
			player.delayTakeDamage = 1.f;
		}
		else if (sprite.preset.name == "aidkit" && player.health < 100) {
			player.health += 25;
			continue;
		}
		else if (sprite.preset.name == "armour") {
			player.armour += 25;
			continue;
		}
		else if (sprite.preset.name == "ammo") {
			player.ammo += 10;
			continue;
		}

		level.spriteObjects[n] = sprite;
		n++;
	}
	level.countOfSpriteObjects = n;
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
		n += 1;
	}
	player.countOfWallsAround = n;

	n = 0;
	for (int i = 0; i < level.countOfSpriteObjects; i++) {
		SpriteObject& obj = level.spriteObjects[i];
		float dis = getDistance(obj.middlePos, player.position);
		obj.lastDistanceToPlayer = dis;

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

	Player::animations[0].texture.loadFromFile("assets/shotgun.png");
	Player::animations[1].texture.loadFromFile("assets/shotgun.png");

	createPresets();

	animation = &animations[0];
}

void Player::createPresets() {
	Vector2f wsize = Vector2f(MAX_WIDTH, MAX_HEIGHT);
	Animation* anim = &animations[0];
	anim->type = "hold";
	anim->countOfFrames = 1;
	anim->switchDelay = 0.25;
	anim->frames[0].setParameters(0, "hold", Vector2i(56, 65), Vector2i(103, 84), 0, 0);

	anim = &animations[1];
	anim->type = "shot&reload";
	anim->countOfFrames = 12;
	anim->switchDelay = 0.125;
	anim->frames[0].setParameters(0, "shot", Vector2i(171, 40), Vector2i(116, 109), 0, 0);
	anim->frames[1].setParameters(0, "reload", Vector2i(295, 45), Vector2i(95, 106), 0, 0);
	anim->frames[2].setParameters(0, "reload", Vector2i(408, 28), Vector2i(96, 127), 0, 0);
	anim->frames[3].setParameters(0, "reload", Vector2i(518, 7), Vector2i(105, 153), 0, 0);
	anim->frames[4].setParameters(0, "reload", Vector2i(636, 2), Vector2i(104, 160), 0, 0);
	anim->frames[5].setParameters(0, "reload", Vector2i(103, 156), Vector2i(106, 139), 0, 0);
	anim->frames[6].setParameters(0, "reload", Vector2i(235, 201), Vector2i(90, 93), 0, 0);
	anim->frames[7].setParameters(0, "reload", Vector2i(363, 228), Vector2i(76, 68), 0, 0);
	anim->frames[8].setParameters(0, "reload", Vector2i(452, 229), Vector2i(76, 69), 0, 0);
	anim->frames[9].setParameters(0, "reload", Vector2i(557, 199), Vector2i(90, 93), 0, 0);
	anim->frames[10].setParameters(0, "reload", Vector2i(675, 179), Vector2i(106, 139), 0, 0);
	anim->frames[11].setParameters(0, "hold", Vector2i(56, 65), Vector2i(103, 84), 0, 0);
}

void Player::changeFrame(float time) {
	Animation* anim = animation;

	if (anim->type == "hold") return;

	if (anim->currentFrame == anim->countOfFrames) {
		anim->currentFrame = 0;
		anim->lastTime = 0;
		animation = &animations[0];

		return;
	}

	anim->lastTime += time;
	if (anim->lastTime > anim->switchDelay) {
		anim->lastTime -= anim->switchDelay;
		anim->currentFrame++;
	}
}

void State::set_type(string name) {
	State::name = name;
}

void SpriteObject::setParameters(float x1, float y1, float length, SpritesPreset& preset) {
	SpriteObject::preset = preset;

	SpriteObject::middlePos = Vector2f(x1, y1);

	SpriteObject::length = length;
}

void Wall::set_parameters(Vector2f leftPos, Vector2f rightPos, SpritesPreset& preset) {
	Wall::preset = preset;

	Wall::leftPos = leftPos;
	Wall::rightPos = rightPos;

	Vector2f tmp = rightPos - leftPos;
	Wall::middlePos = Vector2f(tmp.x / 2, tmp.y / 2) + leftPos;

	Wall::length = getDistance(leftPos, rightPos);
}

void SpritesPreset::setParameters(short type, std::string name,
	Vector2i pos, Vector2i size, float height, short shift) {
	SpritesPreset::type = type;
	SpritesPreset::name = name;

	positionInTileMap = pos;
	sizeInTileMap = size;

	defaultHeight = height;
	heightShift = shift;
}