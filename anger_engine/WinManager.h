#ifndef ANGER_WINMAN
#define ANGER_WINMAN
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <memory>
#include "ResourceManager.h"
#include <functional>

namespace anger{
    class WinManager : public sf::RenderWindow{
        std::shared_ptr<anger::ResourceManager> R;

        public:
            void onWinResize(sf::Event::SizeEvent new_size);
            WinManager(std::shared_ptr<anger::ResourceManager> r, std::string title);
            void loop();
            sf::Vector2u getScreenResolution();

            std::function<void()> onCreate;
            std::function<void(int)> onUpdate;
            sf::Vector2i getMousePos();
            void setMousePos(sf::Vector2i pos);
            sf::Sprite canvas;
    };
}
#endif