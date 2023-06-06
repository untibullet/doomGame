#pragma once

#include <string>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

using sf::Vector2f;

namespace objects {
	const short FOV = 90;
	const short COUNT_OF_RAYS = 180;

	const float MIN_DEVIATION = 0.000001;
	const short NORMAL_DISTANCE = 4;
	const float NORMAL_WALL_SIZE = 0.3;

	const int MAX_COUNT_OF_WALLS = 100;
	const short MAX_WALLS_AROUND = MAX_COUNT_OF_WALLS / 4;

	const short COUNT_OF_LEVELS = 10;

	const std::string START_LEVEL = "level1.txt";

	class Wall {
	public:
		Wall(Vector2f leftPos, Vector2f rightPos);

		Vector2f leftPos;
		Vector2f rightPos;
		float length;
	};

	class Level {
	public:
		std::string levelAddres;
		std::string levelName;
		Wall walls[MAX_COUNT_OF_WALLS];
	};

	class State {
	public:
		void set_type(std::string name);

		std::string name;
	};

	class Player {
	public:
		Player();

		void set_default(Vector2f position, float angle, short fov);

		short fov;
		float rfov;
		Vector2f position;
		float centralAngle;
		Vector2f direction;
		int countOfWallsAround = 0;
		Wall wallsAround[MAX_WALLS_AROUND];

	};

	class Game {
	public:
		Game();

		Level loadLevel(std::string levelName);

		void render(sf::RenderWindow& win);

		void drawBackGround(sf::RenderWindow& win);

		void drawWalls(sf::RenderWindow& win);

		Vector2f checkCrossing(Vector2f startPos, float angle, Vector2f p1, Vector2f p2, bool& isCrossing);

		float getDistance(Vector2f p1, Vector2f p2);

		void drawSprites(sf::RenderWindow& win);

		void drawInterface(sf::RenderWindow& win);

		void update();

		template <typename T>
		void movementController(T movingObject, float angle, __time32_t time);

		Player player = Player();
		State state = State();
		std::string levels[COUNT_OF_LEVELS];
		Level level = loadLevel(START_LEVEL);
		float deltaAngle;
		float renderingRadius = 10;
	};
}