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
	const short COUNT_OF_RAYS = 720;

	const float MIN_DEVIATION = 0.000001;
	const short NORMAL_DISTANCE = 4;
	const float NORMAL_WALL_SIZE = 0.3;

	const int MAX_COUNT_OF_WALLS = 100;
	const short MAX_WALLS_AROUND = MAX_COUNT_OF_WALLS / 4;

	const short MAX_COUNT_OF_SPRITES = 32;
	const short MAX_SPRITES_AROUND = 16;

	const short MAX_COUNT_OF_FRAMES = 16;

	const short COUNT_OF_LEVELS = 10;

	const std::string START_LEVEL = "levels/level1.txt";

	const float PLAYER_SPEED = 2.5;
	const float INTERACTION_RADIUS = 1.f;

	class SpritesPreset {
	public:
		void setParameters(short type, std::string name,
			Vector2i pos, Vector2i size, float height, short shift);

		short type;
		std::string name = "dsfgsfg";

		Vector2i positionInTileMap;
		Vector2i sizeInTileMap;

		float defaultHeight;
		short heightShift;
	};

	class Animation {
	public:
		std::string type;

		bool inProcess = false;

		sf::Texture texture;

		short countOfFrames;
		short currentFrame = 0;

		float lastTime = 0;
		float switchDelay;

		SpritesPreset frames[MAX_COUNT_OF_FRAMES];
	};

	class Polygon {
	public:
		SpritesPreset preset;

		Vector2f leftPos;
		Vector2f rightPos;
		Vector2f middlePos;
		float length;
	};

	class Wall : public Polygon {
	public:
		void set_parameters(Vector2f leftPos, Vector2f rightPos, 
			SpritesPreset& preset);
	};

	class SpriteObject : public Polygon {
	public:
		void setParameters(float x, float y, float length,
			SpritesPreset& preset);

		float lastDistanceToPlayer;
		Vector2f pointOfCollision;

		float health = 100;
		bool isAlive = true;
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

		void createPresets();

		void changeFrame(float time);

		short fov;
		float rfov;
		float centralAngle;
		Vector2f direction;

		float speed = PLAYER_SPEED;

		float delayTakeDamage = 0;

		Vector2f position;

		short health = 100;
		short armour = 0;
		short ammo = 10;
		float damage = 65.f;

		short kills = 0;
		
		int countOfWallsAround = 0;
		Wall wallsAround[MAX_WALLS_AROUND];

		int countOfSpriteObjectsAround;
		SpriteObject spriteObjectsAround[MAX_COUNT_OF_SPRITES];

		Animation animations[2];
		Animation* animation = &animations[0];
	};

	class Game {
	public:
		Game(sf::RenderWindow& win);

		Level loadLevel(std::string levelName);

		void createPresets();

		void render(sf::RenderWindow& win, float time);

		void drawBackGround(sf::RenderWindow& win);

		void drawWalls(sf::RenderWindow& win);

		Wall findVisibleWall(bool& haveWall, Vector2f& crossingPoint, float& distance, float angle);

		SpriteObject* findVisibleSpriteObjects(int& count, float angle, float maxRenderDistance);

		Vector2f checkCrossing(Vector2f startPos, float angle, Vector2f p1, Vector2f p2, bool& isCrossing);

		void drawObjects(sf::RenderWindow& win);

		void drawInterface(sf::RenderWindow& win);

		void drawCrosshair(sf::RenderWindow& win);

		void drawWeapon(sf::RenderWindow& win);

		void drawStats(sf::RenderWindow& win);

		template <typename T>
		void drawSprite(sf::RenderWindow& win, T& object, float distance, Vector2f point,
			float maxAngle, float angle, short ray);

		template <typename T>
		void createPolygon(T object, sf::Sprite& polygon, Vector2f point, float angle);

		void update(float time);

		void updateRenderingArrays();

		void updateSpriteObjectsAround();

		void viewController(sf::RenderWindow& win, sf::Mouse& mouse, float time);

		template <typename T>
		void rotate(T& object, short width, short delta, float time);

		void movementController(sf::Keyboard::Key key, float time);

		template <typename T>
		void move(T& object, float dx, float dy, float time);

		void shoot();

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

		sf::Texture texturePack[4];
		SpritesPreset presets[16];
	};
}