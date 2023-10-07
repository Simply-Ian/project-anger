#ifndef PROJANGER_MYRMAN
#define PROJANGER_MYRMAN
#include "anger_engine/ResourceManager.h"
#include <SFML/Graphics/Font.hpp>

class MyRMan : public anger::ResourceManager{
    /// @brief Зависимость MyRMan::load_settings_file(). Устанавливает заданному полю заданное значение при чтении файла настроек,
    /// при необходимости -- предварительно конвертирует значение.
    /// @param ident название опции в файле настроек
    /// @param value значение.
    void set_up_field(std::string ident, std::string value);

    public:
        void load_level(std::string path) override;
        void load_settings_file(std::string path) override;
        int mini_map_screensize;
        sf::Color text_color;
        int small_text_size{12};
        double player_speed;
        double player_x = 0;
        double player_y = 0;
        sf::Font font;
        MyRMan();
};
#endif