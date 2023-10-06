#ifndef ANGER_WINMAN
#define ANGER_WINMAN
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <memory>
#include "ResourceManager.h"
#include "ObjectManager.h"

class WinManager{
    sf::RenderWindow win{sf::VideoMode::getDesktopMode(), "Project Anger"};
    sf::Sprite canvas;

    sf::RenderTexture mini_map;
    sf::View mapView;
    sf::Sprite mapSprite;

    void onWinResize(sf::Event::SizeEvent new_size);
    void handleMouse();
    void handleKeyboard(double fl);
    void draw_minimap();
    ResourceManager* R;
    ObjectManager* O;
    sf::Font font;
    public:
        WinManager(ResourceManager* r, ObjectManager* o);
        void loop();
};
#endif