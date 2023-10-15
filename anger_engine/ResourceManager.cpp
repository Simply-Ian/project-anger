#include "ResourceManager.h"
#include <string>
#include <vector>
#include <iostream> // До появления логгера
#include <sstream>

bool anger::Level::valid_coords(sf::Vector2i coords) const{
    return 0 <= coords.x && coords.x < size.x && 0 <= coords.y && coords.y < size.y;
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