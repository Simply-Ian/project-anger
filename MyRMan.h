#ifndef PROJANGER_MYRMAN
#define PROJANGER_MYRMAN
#include "anger_engine/ResourceManager.h"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <map>
#include <vector>
#include <memory>

/// @file MyRMan.h
/// @brief Заголовочный файл, содержащий определения классов MyRMan и MyLevel

/// @brief Класс-наследник anger::Level. Датакласс для хранения уровня. Дополнительно определяет хэш-карту для хранения текстур и 
/// метод для загрузки их из файла
class MyLevel : public anger::Level{
    public:
        MyLevel(std::string n, int w, int h, double br, double dec, double pl_x, double pl_y) : 
            anger::Level(n, w, h, br, dec, pl_x, pl_y){}

        /// @brief Хэш-карта, в которой хранятся все текстуры, используемые в уровне. Ключами служат имена файлов в папке
        /// ./assets/textures
        std::map<std::string, std::shared_ptr<sf::Image>> textures;
        MyLevel() : Level(){}

        /// @brief Метод для загрузки текстур из файлов изображений в MyLevel::textures.
        /// @param textures_names Вектор имен изображений в папке ./assets/textures, которые необходимо загрузить
        void load_textures(std::vector<std::string> textures_names);
};

/// @brief Класс-наследник anger::ResourceManager. Реализует логику загрузки уровня из файла, чтения из файла и хранения настроек,
/// а также числовых, строковых и пр. значений, необходимых другим модулям (аналогично R в Android)
class MyRMan : public anger::ResourceManager{
    /// @brief Зависимость MyRMan::load_settings_file(). Устанавливает заданному полю заданное значение при чтении файла настроек,
    /// при необходимости -- предварительно конвертирует значение.
    /// @param ident название опции в файле настроек
    /// @param value значение.
    void set_up_field(std::string ident, std::string value);

    public:
        /// @brief Текущий уровень
        MyLevel lvl;

        /// @brief Метод для чтения и парсинга .lvl файла с последующей инициализацией MyRMan::lvl
        /// @param path путь к файлу .lvl
        void load_level(std::string path) override;

        /// @brief Метод для чтения и парсинга файла настроек
        /// @param path путь к файлу настроек
        void load_settings_file(std::string path) override;

        /// @brief Зависимость MyRMan::load_level(). Загружает блоки стен и заполняет MyRMan::lvl.lvl_map
        /// @param j nlohmann::json объект
        void load_walls(auto j);

        /// @brief Зависимость MyRMan::load_level(). Загружает плитки пола и заполняет MyRMan::lvl.floor_tiles.
        /// @param j nlohmann::json объект
        void load_floor(auto j);

        /// @brief Видимый размер миникарты на экране (в px)
        int mini_map_screensize;
        /// @brief Цвет выводимого на экран текста
        sf::Color text_color;
        /// @brief Размер выводимого на экран текста
        int small_text_size{12};
        /// @brief Скорость движения игрока (в блоках/с)
        double player_speed;
        /// @brief Длина стороны collision box-а игрока (это квадрат)
        double collision_box_size;
        /// @brief Шрифт, которым выводится на экран весь текст
        sf::Font font;
        MyRMan();
};
#endif