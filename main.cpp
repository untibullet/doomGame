#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
using sf::Mouse;
using sf::Keyboard;

using std::string;

using objects::Game;

const short MAX_FPS = 60;
const short ONE_SEC_IN_MICROSEC = 1000;

const short COUNT_OF_WALLS = 4;

const short MAX_WIDTH = 1280, MAX_HEIGHT = 720;

float mouseSpeedCoef = 0.75;

int main()
{
    short width = MAX_WIDTH, height = MAX_HEIGHT;

    sf::RenderWindow window(sf::VideoMode(width, height), "MyDoom[*_*]");

    Mouse mouse = Mouse();
    window.setMouseCursorVisible(false);

    Game game = Game(window);

    sf::Clock clock;
    int fps = 0;
    float timePerShot = 0; // seconds

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();

        fps++;
        timePerShot += time;
        if (timePerShot > 1) {
            timePerShot -= 1;
            fps = 0;
        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        game.viewController(window, mouse, time);

        if (Keyboard::isKeyPressed(Keyboard::W)) {
            game.movementController(Keyboard::W, time);
        }
        if (Keyboard::isKeyPressed(Keyboard::S)) {
            game.movementController(Keyboard::S, time);
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            game.movementController(Keyboard::A, time);
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            game.movementController(Keyboard::D, time);
        }

        mouse.setPosition(game.centralMousePos);

        game.update(time);

        window.clear();

        game.render(window, time);

        window.display();
    }

    return 0;
}