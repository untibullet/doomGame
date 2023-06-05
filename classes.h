#pragma once

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

namespace objects {

	const int MAX_COUNT_OF_WALLS = 100;

	class Wall {
	public:
		Wall(sf::Vector2f position, sf::Vector2f size);

		sf::Vector2f position;
		sf::Vector2f size;
	};

	class Level {
	public:
		std::string levelAddres;
		std::string levelName;
		Wall walls[MAX_COUNT_OF_WALLS];
	};

	class State {
	public:
		void set_type(std::string name) {
			State::name = name;
		}

		std::string name;
	};

	class Player {
	public:
		void set_default(sf::Vector2f position, float angle, short fov);

		sf::Vector2f position;
		float centralAngle;
		sf::Vector2f direction;
		short fov;
		float rfov;
	};

	class Game {
	public:
		Game();

		void render(sf::RenderWindow& win);

		void drawBackGround(sf::RenderWindow& win);

		void drawWalls(sf::RenderWindow& win);

		void drawSprites(sf::RenderWindow& win);

		void drawInterface(sf::RenderWindow& win);

		void update();

		void setPlayerAttributes();

		void movementController();

		Player player = Player();
		State state = State();
		float deltaAngle;
	};
}