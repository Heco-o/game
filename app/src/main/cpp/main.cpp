#include <SFML/Graphics.hpp>

// Android/SFML smoke test.
// Once this builds and installs, replace this file with the Android-adapted
// version of your game source. See README.md before copying the desktop source.

int main()
{
    sf::RenderWindow window(sf::VideoMode({1280u, 700u}), "Maze Game");

    sf::RectangleShape player({24.f, 24.f});
    player.setFillColor(sf::Color::Blue);
    player.setPosition({100.f, 100.f});

    sf::RectangleShape wall({500.f, 12.f});
    wall.setFillColor(sf::Color(80, 80, 80));
    wall.setPosition({100.f, 300.f});

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::White);
        window.draw(wall);
        window.draw(player);
        window.display();
    }

    return 0;
}
