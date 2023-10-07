#include "miniMap.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

void miniMap::prepare_minimap(){
    mapView = sf::View{sf::FloatRect{0, 0, R->mini_map_screensize, R->mini_map_screensize}};
    texture.create(R->lvl.size.x * 30, R->lvl.size.y * 30);
    texture.setView(mapView);
    mapSprite.setPosition(5, 5);
    mapSprite.setScale({R->mini_map_screensize / texture.getSize().x, R->mini_map_screensize / texture.getSize().x});
}

sf::Sprite& miniMap::draw_minimap(sf::Vector2f player_pos){
    const float block_size = 30.f;
    // SFML использует систему координат с Oy, направленной вниз; система координат движка -- с Oy, направленной вверх.
    const float player_screen_x = player_pos.x * block_size;
    const float player_screen_y = inverse_y(player_pos.y * block_size);
    mapView.setCenter({player_screen_x, player_screen_y});
    texture.setView(mapView);

    texture.clear(sf::Color::Black);

    draw_blocks();
    draw_player(player_screen_x, player_screen_y);
    draw_zero_mark();

    // Рисуем видимые поверхности
    // for (double offset = 0; offset < R->screen_res.x; offset++){
    //     sf::Vector2f touch_pos = O->cam.get_touchdown_coords(offset * (1.0 / R->screen_res.x)).pos;
    //     sf::RectangleShape dot{{2, 2}};
    //     dot.setFillColor(sf::Color::Magenta);
    //     dot.setPosition(touch_pos.x * block_size, inverse_y(touch_pos.y * block_size));
    //     texture.draw(dot);
    // }

    texture.display();
    mapSprite.setTexture(texture.getTexture());
    return mapSprite;
}

void miniMap::draw_player(float player_screen_x, float player_screen_y){
    sf::CircleShape player_dot(4.f);
    player_dot.setOrigin(player_dot.getRadius() / 2, player_dot.getRadius() / 2);
    player_dot.setFillColor(sf::Color::Green);
    player_dot.setPosition(sf::Vector2f{player_screen_x, player_screen_y});
    texture.draw(player_dot);
}

void miniMap::draw_zero_mark(){
    sf::Text zero{"(0, 0)", R->font, R->small_text_size};
    zero.setFillColor(R->text_color);
    zero.setPosition(0, inverse_y(0));
    texture.draw(zero);
}

void miniMap::draw_blocks(){
    for (int x = 0; x < R->lvl.size.x; x++){
        for (int y = 0; y < R->lvl.size.y; y++){
            anger::Block bl = R->lvl.get_block(x, y);
            if (!bl.is_a_null_block){
                sf::RectangleShape rect({block_size, block_size});
                rect.setFillColor(bl.color);
                rect.setPosition(x * block_size, inverse_y(y * block_size) - block_size);
                texture.draw(rect);
            }
        }
    }
}

int miniMap::inverse_y(int it_y){
    return texture.getSize().y - it_y;
}