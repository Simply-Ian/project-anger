#include "ResourceManager.h"
#include <string>
#include <vector>
#include <iostream> // До появления логгера
#include <sstream>

anger::Level::Level(std::string n, int w, int h, double br, double dec, double pl_x, double pl_y) : name(n), size(w, h), 
                        brightness(br), decay_factor(dec), player_x(pl_x), player_y(pl_y){
    lvl_map = std::make_unique<Block[]>(w * h);
    floor_tiles = std::make_unique<std::shared_ptr<sf::Image>[]>(w * h);
}

anger::Block anger::Level::get_block(int x, int y) const {
    if (x >= 0 && size.x > x && y >= 0 && size.y > y)
        return lvl_map[y * size.x + x];
    else throw std::range_error{std::string("Invalid block position: x=") + std::to_string(x) 
        + ", y=" + std::to_string(y)};
}

void anger::Level::set_block(const anger::Block b, int x, int y){
    if (0 <= x && x < size.x){
        if (0 <= y && y < size.y){
            lvl_map[y * size.x + x] = b;
            return;
        }
    }
    std::cout << "Unable to set block (x: " << x << ", y: " << y << "). Invalid position" << std::endl;
}

bool anger::Level::valid_coords(sf::Vector2f coords) const{
    return 0 <= coords.x && coords.x < size.x && 0 <= coords.y && coords.y < size.y;
}

std::shared_ptr<sf::Image> anger::Level::get_tile(int x, int y) const {
    if (valid_coords({x, y}))
        return floor_tiles[y * size.x + x];
    else {
        // std::cout << std::string("Invalid tile position: x=") + std::to_string(x) 
        //                         + ", y=" + std::to_string(y) << std::endl;
        return nullptr;
    }
}

void anger::Level::set_tile(std::shared_ptr<sf::Image> tile, int x, int y){
    if (valid_coords({x, y})){
        floor_tiles[y * size.x + x] = tile;
        return;
    }
    std::cout << "Unable to set tile (x: " << x << ", y: " << y << "). Invalid position" << std::endl;
}

sf::Color anger::color_from_hex(std::string hex){
    if (hex.size() >= 7){
        const char alphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        std::string raw_red = hex.substr(1, 2);
        std::string raw_green = hex.substr(3, 2);
        std::string raw_blue = hex.substr(5, 2);
        auto find = [&](char symbol) -> int {
            for (int i = 0; i < 16; i++)
                if (alphabet[i] == symbol) return i;
            return 15; // Все значения больше f интерпретируются как f
        };
        return sf::Color(find(raw_red[0]) * 16 + find(raw_red[1]),
                        find(raw_green[0]) * 16 + find(raw_green[1]),
                        find(raw_blue[0]) * 16 + find(raw_blue[1]));
    }
    else{
        std::cout << "Bad color std::string: " << hex << std::endl;
        return {0, 0, 0};
    }
}