#ifndef ANGER_RM
#define ANGER_RM
#include <string>
#include <memory>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "nlohmann/json.hpp"
#include <iostream>

sf::Color color_from_hex(std::string hex);

struct Block{
    sf::Color color;
    bool is_a_null_block=false;
    Block(sf::Color c, bool is_null) : color(c), is_a_null_block(is_null) {}
    Block() : color(sf::Color::White), is_a_null_block(true){}
};

struct ResourceManager{
    struct Level{
        private:
            std::unique_ptr<Block[]> lvl_map;
        
        public:
            std::string name;
            sf::Vector2i size;
            sf::Color skycolor;

            Level(std::string n, int w, int h, sf::Color skclr) : name(n), size(w, h), skycolor(skclr){
                lvl_map = std::make_unique<Block[]>(w * h);
            }

            Level(){}

            Block get_block(int x, int y){
                return lvl_map[y * size.x + x];
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

    Level lvl;
    sf::Vector2u screen_res;
    bool is_paused;
    // Угол зрения в градусах
    double FOV;
    int mini_map_screensize;
    sf::Color text_color;
    int small_text_size{12};
    double player_speed;

    void load_settings_file();
    void load_level(std::string path);
};
#endif