#include "WinManager.h"
#include <cmath>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "os/exec.h"
#include "os/get_default_font_filename.h"

WinManager::WinManager(ResourceManager* r, ObjectManager* o) : R(r), O(o){
    R->screen_res = win.getSize();
    win.setMouseCursorVisible(false);
    // Чтобы процессор не был чрезмерно загружен
    win.setFramerateLimit(200);
    font.loadFromFile(get_default_font_filename());
    mapView = sf::View{sf::FloatRect{0, 0, R->mini_map_screensize, R->mini_map_screensize}};
    mini_map.create(R->lvl.size.x * 10, R->lvl.size.y * 10);
    mini_map.setView(mapView);

    mapSprite.setPosition(5, 5);
    mapSprite.setScale({R->mini_map_screensize / mini_map.getSize().x, R->mini_map_screensize / mini_map.getSize().x});
}

void WinManager::loop(){
    sf::Clock q;
    int framelength;
    sf::Text FPS_label("0", font);
    sf::Text player_pos_label("", font);
    sf::Text player_angle_label("", font);
    FPS_label.setPosition(5, R->mini_map_screensize + 5);
    FPS_label.setCharacterSize(R->small_text_size);
    FPS_label.setFillColor(R->text_color);

    player_pos_label.setPosition(5, FPS_label.getPosition().y + FPS_label.getGlobalBounds().height + 5);
    player_pos_label.setCharacterSize(R->small_text_size);
    player_pos_label.setFillColor(R->text_color);
    int FPS = 0;
    while(win.isOpen()){
        q.restart();
        win.clear(sf::Color::Black);
        sf::Event event;
        while(win.pollEvent(event)){
            if (event.type == sf::Event::Closed) win.close();

            else if(event.type == sf::Event::KeyPressed){
                if(event.key.code == sf::Keyboard::Escape) {
                    R->is_paused = !R->is_paused;
                    win.setMouseCursorVisible(R->is_paused);
                }
            }

            else if (event.type == sf::Event::Resized)
                onWinResize(event.size);
        }

        if(!R->is_paused){
            handleKeyboard(framelength);
            handleMouse();
        }
        
        FPS = std::floor(1000 / framelength);
        FPS_label.setString("FPS: " + std::to_string(FPS));

        player_pos_label.setString("x: " + to_string(O->player.getPos().x) + ", y=" + to_string(O->player.getPos().y) +
                                     ", a=" + to_string(O->player.get_angle_degs()) + "\ndirx: " + to_string(O->player.dir.x) + 
                                     ", diry: "  + to_string(O->player.dir.y));
        buffer.display();
        canvas.setTexture(buffer.getTexture());
        win.draw(canvas);
        win.draw(FPS_label);
        win.draw(player_pos_label);

        draw_minimap();

        win.display();
        framelength = q.getElapsedTime().asMilliseconds();
    }
}

void WinManager::handleMouse(){
    const int mouse_border_width = 25;
    const int win_w_without_borders = R->screen_res.x - 2*mouse_border_width;
    int cur_mouse_x = sf::Mouse::getPosition(win).x;

    O->player.set_angle_mouse(cur_mouse_x, win_w_without_borders, mouse_border_width);

    if(cur_mouse_x > R->screen_res.x - mouse_border_width){
        sf::Mouse::setPosition(sf::Vector2i(mouse_border_width, R->screen_res.y/2), win);
    }
    else if(cur_mouse_x < mouse_border_width){
        sf::Mouse::setPosition(sf::Vector2i(R->screen_res.x - mouse_border_width, R->screen_res.y/2), win);
    }
}

void WinManager::onWinResize(sf::Event::SizeEvent new_size){
    sf::Vector2u win_size = win.getSize();
    win.setView(sf::View({0.f, 0.f, static_cast<float>(win_size.x),
                                         static_cast<float>(win_size.y)}));
}

void WinManager::handleKeyboard(double fl){
    int ad_key = 0;
    int ws_key = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        ws_key = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        ws_key = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        ad_key = -1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        ad_key = 1;
    
    O->player.move(ws_key, ad_key, fl);
}

void WinManager::draw_minimap(){
    const float block_size = 10.f;
    // SFML использует систему координат с Oy, направленной вниз; система координат движка -- с Oy, направленной вверх.
    const float player_screen_x = O->player.getPos().x * block_size;
    const float player_screen_y = mini_map.getSize().y - O->player.getPos().y*block_size;
    mapView.setCenter({player_screen_x, player_screen_y});
    mini_map.setView(mapView);

    mini_map.clear(sf::Color::Black);
    for (int x = 0; x < R->lvl.size.x; x++){
        for (int y = 0; y < R->lvl.size.y; y++){
            Block bl = R->lvl.get_block(x, y);
            if (!bl.is_a_null_block){
                sf::RectangleShape rect({block_size, block_size});
                rect.setFillColor(bl.color);
                rect.setPosition(x * block_size, mini_map.getSize().y - y * block_size);
                mini_map.draw(rect);
            }
        }
    }
    sf::CircleShape player_dot(9, 3);
    player_dot.setOrigin(9, 9);
    player_dot.setFillColor(sf::Color(0, 255, 0));
    player_dot.setPosition(sf::Vector2f{player_screen_x, player_screen_y});
    player_dot.setRotation(-O->player.get_angle_degs() + 90); // Треугольник по умолчанию смотрит углом вверх
    mini_map.draw(player_dot);
    mini_map.display();

    mapSprite.setTexture(mini_map.getTexture());
    win.draw(mapSprite);
}