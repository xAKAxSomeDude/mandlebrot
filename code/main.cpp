#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"

int main()
{
    Event event;
    int pixelWidth = VideoMode::getDesktopMode().width;
    int pixelHeight = VideoMode::getDesktopMode().height;
    RenderWindow window(VideoMode(pixelWidth, pixelHeight), "Mandelbrot Set", Style::Default);
    ComplexPlane mb(pixelWidth, pixelHeight);
    Font font;
    font.loadFromFile("Oblata.otf");
    Text hud;
    hud.setFont(font);
    hud.setCharacterSize(20);
    hud.setFillColor(Color::White);
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                cout << "Window closed with close button" << endl;
                window.close();
            }
            if (event.type == Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == Mouse::Left)
                {
                    mb.setMouseLocation(Mouse::getPosition(window));
                    Vector2i mousePos = Mouse::getPosition(window);
                    Vector2f worldPos = window.mapPixelToCoords(mousePos);
                    mb.setCenter(worldPos);
                    mb.zoomIn();
                }
                if (event.mouseButton.button == Mouse::Right)
                {
                    mb.setMouseLocation(Mouse::getPosition(window));
                    Vector2i mousePos = Mouse::getPosition(window);
                    Vector2f worldPos = window.mapPixelToCoords(mousePos);
                    mb.setCenter(worldPos);
                    mb.zoomOut();
                }
            }
            if (event.type == Event::MouseMoved)
            {
                mb.setMouseLocation(Mouse::getPosition(window));
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                cout << "Escape key used to close program" << endl;
                window.close();
            }
        }
        mb.updateRender();
        mb.loadText(hud);
        window.clear();
        window.draw(mb);
        window.draw(hud);
        window.display();
    }
}
