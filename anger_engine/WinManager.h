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
    /// @brief Класс, представляющий собой окно. В этом окне отрисовывается изображение, оно же обрабатывает движения мыши,
    /// ресайз. Главный цикл игры запускается в методе этого класса anger::WinManager::loop
    class WinManager : public sf::RenderWindow{
        std::shared_ptr<anger::ResourceManager> R;

        public:
            void onWinResize(sf::Event::SizeEvent new_size);
            WinManager(std::shared_ptr<anger::ResourceManager> r, std::string title);
            /// @brief Метод содержит главный цикл игры. 
            /// Каждую итерацию засекается время старта, вызывается anger::WinManager::onUpdate, отрисовывается содержимое
            /// окна и фиксируется длительность кадра.
            void loop();
            sf::Vector2u getScreenResolution();

            /// @brief Callback, назначаемый разработчиком игры. Вызывается каждую итерацию главного цикла (т. е. каждый кадр).
            std::function<void(int)> onUpdate;
            sf::Vector2i getMousePos();
            void setMousePos(sf::Vector2i pos);
    };
}
#endif