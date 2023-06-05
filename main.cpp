#include <SFML/Graphics.hpp>

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "func.h"
#include "classes.h"

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

    Vector2f size = Vector2f(2.f, 2.f);
    Wall walls[] = { Wall(Vector2f(5.f, 8.f), size), Wall(Vector2f(5.f, 10.f), size),
    Wall(Vector2f(9.f, 9.f), size), Wall(Vector2f(11.f, 7.f), size) };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.centralAngle += 0.1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.centralAngle -= 0.1;
        }

        game.update();

        window.clear();

        game.render(window);

        window.display();
    }

    return 0;
}