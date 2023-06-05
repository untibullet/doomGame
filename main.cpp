#include <iostream>
#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

#include "func.h"
#include "classes.h"

using sf::Vector2f;
using sf::Vector2i;

const short FOV = 90;
const short MAX_FPS = 60;
const short COUNT_OF_RAYS = 180;
const short NORMAL_DISTANCE = 4;
const float NORMAL_WALL_SIZE = 0.3;

const short COUNT_OF_WALLS = 4;

const short MAX_WIDTH = 1280, MAX_HEIGHT = 720;

int main()
{
    float fov = (float)(FOV * M_PI) / 180;
    short width = MAX_WIDTH, height = MAX_HEIGHT;

    short imStep = width / COUNT_OF_RAYS;

    sf::RenderWindow window(sf::VideoMode(width, height), "MyDoom[*_*]");

    Player player;
    player.set_default(Vector2f(8.f, 3.f), M_PI / 2, fov);

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
            player.angle += 0.1;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.angle -= 0.1;
        }

        window.clear();

        float step = fov / COUNT_OF_RAYS;

        Vector2f dirVector;
        for (short i = 0; i < COUNT_OF_RAYS; i++) {
            float maxAngle = player.angle + fov / 2;
            float nowAngle = maxAngle - step * i;
            dirVector = Vector2f(cos(nowAngle), sin(nowAngle));

            if (dirVector.x == 0 or dirVector.y == 0)
                continue;

            for (int j = 0; j < COUNT_OF_WALLS; j++) {
                Wall rect = walls[j];
                float x, x1;
                float y, y1;
                float tmp;
                
                x = (rect.position.x - player.position.x) / dirVector.x;
                x1 = (rect.position.x - player.position.x + rect.size.x) / dirVector.x;
                if (dirVector.x < 0) {
                    tmp = x;
                    x = x1;
                    x1 = tmp;
                }

                y = (rect.position.y - player.position.y) / dirVector.y;
                y1 = (rect.position.y - player.position.y + rect.size.y) / dirVector.y;
                if (dirVector.y < 0) {
                    tmp = y;
                    y = y1;
                    y1 = tmp;
                }

                bool flag1 = x < y and x < y1 and x1 < y and x1 < y1;
                bool flag2 = y < x and y < x1 and y1 < x and y1 < x1;
                if (!flag1 and !flag2) {
                    float k = std::max(x, y);
                    short rectHeight = (((float)NORMAL_DISTANCE / k) * (height * NORMAL_WALL_SIZE));
                    short rectY = height / 2 - rectHeight / 2;
                    sf::RectangleShape rectShape = sf::RectangleShape(Vector2f(imStep, rectHeight));
                    rectShape.setFillColor(sf::Color::Blue);
                    rectShape.setPosition(i * imStep, rectY);
                    window.draw(rectShape);
                    // break;
                }
            }
        }

        window.display();
    }

    return 0;
}