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

            Level(std::string n, int w, int h, double br, double dec, double pl_x, double pl_y);

            Level(){}

            Block get_block(int x, int y) const;

            void set_block(const Block b, int x, int y);

            std::shared_ptr<sf::Image> get_tile(int x, int y) const;

            void set_tile(std::shared_ptr<sf::Image> tile, int x, int y);
            
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