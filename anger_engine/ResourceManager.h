#ifndef ANGER_RM
#define ANGER_RM
#include <string>
#include <memory>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "nlohmann/json.hpp"
#include <iostream>
#include <exception>

namespace anger{
    struct Block{
        sf::Color color;
        bool is_a_null_block=false;
        Block(sf::Color c, bool is_null) : color(c), is_a_null_block(is_null) {}
        Block() : color(sf::Color::White), is_a_null_block(true){}
    };
    
    struct Level{
        private:
            std::unique_ptr<Block[]> lvl_map;

        public:
            std::string name;
            sf::Vector2i size;
            sf::Color skycolor;
            double player_x;
            double player_y;
            double brightness;
            double decay_factor;

            Level(std::string n, int w, int h, sf::Color skclr, double br, double dec, double pl_x, double pl_y) : name(n), size(w, h), 
                        skycolor(skclr), brightness(br), decay_factor(dec), player_x(pl_x), player_y(pl_y){
                lvl_map = std::make_unique<Block[]>(w * h);
            }

            Level(){}

            Block get_block(int x, int y) const {
                if (x >= 0 && size.x > x && y >= 0 && size.y > y)
                    return lvl_map[y * size.x + x];
                else throw std::range_error{std::string("Invalid block position: x=") + std::to_string(x) 
                                            + ", y=" + std::to_string(y)};
            }

            void set_block(const Block b, int x, int y){
                if (0 <= x && x < size.x){
                    if (0 <= y && y < size.y){
                        lvl_map[y * size.x + x] = b;
                        return;
                    }
                }
                std::cout << "Unable to set block (x: " << x << ", y: " << y << "). Invalid position" << std::endl;
            }
    };
    sf::Color color_from_hex(std::string hex);

    /// @brief Абстрактный класс, предназначен для хранения и загрузки из файла ресурсов
    struct ResourceManager{
        Level lvl;
        // Угол зрения в градусах
        double FOV;
        double camera_plane_width = 1;

        virtual void load_settings_file(std::string path) = 0;
        virtual void load_level(std::string path) = 0;
    };
}
#endif