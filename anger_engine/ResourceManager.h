#ifndef ANGER_RM
#define ANGER_RM
#include <string>
#include <memory>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "nlohmann/json.hpp"
#include <iostream>
#include <exception>
#include <array>
#include "Block.h"

namespace anger{
    struct Level{
        private:
            std::unique_ptr<Block[]> lvl_map;
            std::unique_ptr<std::shared_ptr<sf::Image>[]> floor_tiles;
        public:
            std::string name;
            sf::Vector2i size;
            double player_x;
            double player_y;
            double brightness;
            double decay_factor;
            double shadow_depth_factor;
            std::shared_ptr<sf::Image> sky;

            Level(std::string n, int w, int h, double br, double dec, double pl_x, double pl_y) : name(n), size(w, h), 
                        brightness(br), decay_factor(dec), player_x(pl_x), player_y(pl_y){
                lvl_map = std::make_unique<Block[]>(w * h);
                floor_tiles = std::make_unique<std::shared_ptr<sf::Image>[]>(w * h);
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

            std::shared_ptr<sf::Image> get_tile(int x, int y) const {
                if (valid_coords({x, y}))
                    return floor_tiles[y * size.x + x];
                else {
                    std::cout << std::string("Invalid tile position: x=") + std::to_string(x) 
                                            + ", y=" + std::to_string(y) << std::endl;
                    return nullptr;
                }

            }

            void set_tile(std::shared_ptr<sf::Image> tile, int x, int y){
                if (valid_coords({x, y})){
                    floor_tiles[y * size.x + x] = tile;
                    return;
                }
                std::cout << "Unable to set tile (x: " << x << ", y: " << y << "). Invalid position" << std::endl;
            }

            /// @brief Проверяет, являются ли переданные координаты действительными (не меньше 0 и не больше соотв. размера уровня)
            bool valid_coords(sf::Vector2f coords) const;
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