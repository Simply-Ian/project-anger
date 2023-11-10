#ifndef PROJANGER_MYRMAN
#define PROJANGER_MYRMAN
#include "anger_engine/ResourceManager.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <map>
#include <vector>
#include <memory>

class MyLevel : public anger::Level{
    public:
        MyLevel(std::string n, int w, int h, double br, double dec, double pl_x, double pl_y) : 
            anger::Level(n, w, h, br, dec, pl_x, pl_y){}
        std::map<std::string, std::shared_ptr<sf::Image>> textures;
        MyLevel() : Level(){}

        void load_textures(std::vector<std::string> textures_names);
};

class MyRMan : public anger::ResourceManager{
    /// @brief Зависимость MyRMan::load_settings_file(). Устанавливает заданному полю заданное значение при чтении файла настроек,
    /// при необходимости -- предварительно конвертирует значение.
    /// @param ident название опции в файле настроек
    /// @param value значение.
    void set_up_field(std::string ident, std::string value);

    public:
        MyLevel lvl;
        void load_level(std::string path) override;
        void load_settings_file(std::string path) override;
        /// @brief Зависимость MyRMan::load_level(). Загружает блоки стен.
        /// @param j nlohmann::json объект
        void load_walls(auto j);
        void load_floor(auto j);
        int mini_map_screensize;
        sf::Color text_color;
        int small_text_size{12};
        double player_speed;
        double collision_box_size;
        sf::Font font;
        MyRMan();
};
#endif