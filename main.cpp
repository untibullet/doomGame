#include "include.h"

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;
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

    sf::Mouse mouse = sf::Mouse();

    Game game = Game();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // game.update();

        window.clear();

        game.render(window);

        window.display();
    }

    return 0;
}