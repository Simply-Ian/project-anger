#include "WinManager.h"
#include <cmath>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
using namespace anger;

WinManager::WinManager(std::shared_ptr<anger::ResourceManager> r, std::string title) : 
            sf::RenderWindow(sf::VideoMode::getDesktopMode(), title){
    R = r;
    setMouseCursorVisible(false);

    // Чтобы процессор не был чрезмерно загружен
    /// @todo Лимит FPS прописывается в настройках
    setFramerateLimit(200);
}

sf::Vector2u WinManager::getScreenResolution(){
    return getSize();
}

sf::Vector2i WinManager::getMousePos(){
    return sf::Mouse::getPosition(*this);
}

void WinManager::setMousePos(sf::Vector2i pos){
    sf::Mouse::setPosition(pos, *this);
}

void WinManager::loop(){
    sf::Clock q;
    int framelength = 5;
    while(isOpen()){
        q.restart();
        onUpdate(framelength);
        display();
        framelength = q.getElapsedTime().asMilliseconds();
    }
}

void WinManager::onWinResize(sf::Event::SizeEvent new_size){
    sf::Vector2u win_size = getSize();
    setView(sf::View({0.f, 0.f, static_cast<float>(win_size.x),
                                         static_cast<float>(win_size.y)}));
}