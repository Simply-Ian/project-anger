#include "Game.h"
#include "anger_engine/conversions/rads_degs.h"
#include <SFML/Graphics/Text.hpp>

Game::Game(std::string level_path){
    R->load_settings_file("settings");
    R->load_level(level_path);

    wm = std::make_unique<anger::WinManager>(R, "Project Anger");
    player = std::make_unique<Player>(R->player_speed, dynamic_cast<anger::Level&>(R->lvl), R->camera_plane_width, 
            0.5 / std::tan(anger::to_rads(R->FOV / 2)) * R->camera_plane_width, wm->getScreenResolution());
    
    // Готовим спрайт, текстуру и view для рисования мини-карты
    mini_map.prepare_minimap();

    prepare_gui();

    // Назначаем коллбэки WinManager-у
    wm->onUpdate = std::bind(&Game::update, this, std::placeholders::_1);
}

void Game::prepare_gui(){
    FPS_label.setFont(R->font);
    FPS_label.setString("0");
    FPS_label.setPosition(5, R->mini_map_screensize + 5);
    FPS_label.setCharacterSize(R->small_text_size);
    FPS_label.setFillColor(R->text_color);

    player_pos_label.setPosition(5, FPS_label.getPosition().y + FPS_label.getGlobalBounds().height + 5);
    player_pos_label.setCharacterSize(R->small_text_size);
    player_pos_label.setFillColor(R->text_color);
    player_pos_label.setFont(R->font);
}

void Game::start(){
    wm->loop();
}

void Game::handleKeyboard(double fl){
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
    
    player->move(ws_key, ad_key, fl);
}

void Game::handleMouse(){
    const int mouse_border_width = 25;
    const int win_width = wm->getScreenResolution().x;
    const int win_half_height = wm->getScreenResolution().y / 2;
    const int win_w_without_borders = win_width - 2*mouse_border_width;
    int cur_mouse_x = wm->getMousePos().x;

    player->set_angle_mouse(cur_mouse_x, win_w_without_borders, mouse_border_width);

    if(cur_mouse_x > win_width - mouse_border_width){
        wm->setMousePos({mouse_border_width, win_half_height});
    }
    else if(cur_mouse_x < mouse_border_width){
        wm->setMousePos({win_width - mouse_border_width, win_half_height});
    }
}

void Game::update(int framelength){
    wm->clear(sf::Color::Black);
    sf::Event event;
    while(wm->pollEvent(event)){
        if (event.type == sf::Event::Closed) wm->close();
        else if(event.type == sf::Event::KeyPressed){
            if(event.key.code == sf::Keyboard::Escape) {
                is_paused = !is_paused;
                wm->setMouseCursorVisible(is_paused);
            }
        }
        else if (event.type == sf::Event::Resized)
            wm->onWinResize(event.size);
    }

    if(!is_paused){
        handleKeyboard(framelength);
        handleMouse();
    }
    
    int FPS = std::floor(1000 / framelength);
    FPS_label.setString("FPS: " + std::to_string(FPS));

    player_pos_label.setString("x: " + std::to_string(player->getPos().x) + ", y=" + std::to_string(player->getPos().y) +
                                 ", a=" + std::to_string(player->get_angle_degs()));
    player->takeImage();
    wm->canvas.setTexture(player->shot);
    wm->draw(wm->canvas);
    wm->draw(FPS_label);
    wm->draw(player_pos_label);

    wm->draw(mini_map.draw_minimap(player->getPos()));
}