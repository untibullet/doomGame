#pragma once

#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

namespace objects {
	const short MAX_WIDTH = 1280, MAX_HEIGHT = 720;

	const short FOV = 90;
	const short COUNT_OF_RAYS = 360;

	const float MIN_DEVIATION = 0.000001;
	const short NORMAL_DISTANCE = 4;
	const float NORMAL_WALL_SIZE = 0.3;

	const int MAX_COUNT_OF_WALLS = 100;
	const short MAX_WALLS_AROUND = MAX_COUNT_OF_WALLS / 4;

	const short MAX_COUNT_OF_SPRITES = 50;
	const short MAX_SPRITES_AROUND = 30;

	const short COUNT_OF_LEVELS = 10;

	const std::string START_LEVEL = "levels/level1.txt";

	const float PLAYER_SPEED = 2;
	const float PLAYER_ROTATIONAL_SPEED = 0.1;

	class Wall {
	public:
		void set_parameters(Vector2f leftPos, Vector2f rightPos);

		short heigthShift = 0;

		Vector2f leftPos;
		Vector2f rightPos;
		Vector2f middlePos;
		float length;
	};

	class SpriteObject {
	public:
		void setParameters(float x1, float y1, short xpick, short ypick, short wdth,
			short hght, float relSize, std::string name);

		std::string type;

		std::string texturePath = "assets/things.png";
		Vector2i positionInTexture;
		Vector2i sizeInTexture;

		short heigthShift = 400;
		float relativeSize = 0.2;

		float length;

		Vector2f middlePos;
		Vector2f leftPos;
		Vector2f rightPos;
	};

	class Level {
	public:
		std::string levelPath;

		Vector2f defaultPlayerPos;

		int countOfWalls;
		Wall walls[MAX_COUNT_OF_WALLS];

		int countOfSpriteObjects;
		SpriteObject spriteObjects[MAX_COUNT_OF_SPRITES];
	};

	class State {
	public:
		void set_type(std::string name);

		std::string name;
	};

	class Player {
	public:
		void set_default(float angle, short fov);

		short fov;
		float rfov;
		float centralAngle;
		Vector2f direction;

		float speed = PLAYER_SPEED;
		float rotationalSpeed = PLAYER_ROTATIONAL_SPEED;

		Vector2f position;
		
		int countOfWallsAround = 0;
		Wall wallsAround[MAX_WALLS_AROUND];

		int countOfSpriteObjectsAround;
		SpriteObject spriteObjectsAround[MAX_COUNT_OF_SPRITES];
	};

	class Game {
	public:
		Game(sf::RenderWindow& win);

		Level loadLevel(std::string levelName);

		void render(sf::RenderWindow& win);

		void drawBackGround(sf::RenderWindow& win);

		void drawWalls(sf::RenderWindow& win);

		Wall findVisibleWall(bool& haveWall, Vector2f& crossingPoint, float& distance, float angle);

		Vector2f checkCrossing(Vector2f startPos, float angle, Vector2f p1, Vector2f p2, bool& isCrossing);

		void drawObjects(sf::RenderWindow& win);

		void drawInterface(sf::RenderWindow& win);

		template <typename T>
		void drawSprite(sf::RenderWindow& win, T& object, float distance, Vector2f point,
			float maxAngle, float angle, short ray);

		template <typename T>
		void createPolygon(T object, sf::Sprite& polygon, Vector2f point, float angle);

		void update();

		void updateRenderingArrays();

		void updateSpriteObjectsAround();

		void movementController(sf::Keyboard::Key key, float time);

		template <typename T>
		void move(T& object, float dx, float dy, float time);

		void viewController(sf::RenderWindow& win, sf::Mouse& mouse, float time);

		template <typename T>
		void rotate(T& object, short width, short delta, float time);

		Vector2u winSize;

		float stepInPixels;
		float wallSize;

		float tanOfHalfFov = tan((FOV * M_PI) / (180 * 2));

		Player player;
		State state;
		std::string levels[COUNT_OF_LEVELS];
		std::string levelPath = START_LEVEL;
		Level level;

		float renderingRadius = 20;
		float deltaAngle;

		float mouseSpeedCoef = 0.75;
		Vector2i centralMousePos = Vector2i(MAX_WIDTH / 2, MAX_HEIGHT / 2);

		sf::Texture wallsTextures;
	};
}