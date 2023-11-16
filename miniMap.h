#ifndef MINIMAP_PROJANGER
#define MINIMAP_PROJANGER
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include "MyRMan.h"
#include <memory>

/// @file miniMap.h
/// @brief Заголовочный файл, содержащий класс miniMap

class miniMap{
    /// @brief Спрайт, на котором рисуется мини-карта
    sf::Sprite mapSprite;

    /// @brief Текстура, на которой рисуется мини-карта
    sf::RenderTexture texture;

    /// @brief View, позволяющий показывать не всю мини-карту целиком, а лишь фрагмент + рисует карту так, что
    /// символ игрока всегда находится визуально в центре мини-карты
    sf::View mapView;
    /// @brief Зависимость draw_minimap(). Рисует на мини-карте стены.
    void draw_blocks();

    /// @brief Зависимость draw_minimap(). Рисует на мини-карте игрока
    void draw_player(float player_screen_x, float player_screen_y);

    /// @brief Зависимость draw_minimap(). Рисует на мини-карте метку (0, 0)
    void draw_zero_mark();

    /// @brief Мини-карта, так же, как и игровой мир, использует систему координат с осью Y вверх. В экранной системе координат
    /// ось Y направлена вниз. Для того, чтобы корректно расположить объект на мини-карте, необходимо с помощью этой функции
    /// перейти от мировой к экранной системе координат.
    /// @param it_y исходная Y-координата
    /// @return Y-координата на оси Y, направленной противоположно исходной
    int inverse_y(int it_y);

    /// @brief экранный размер блока на мини-карте (в px)
    const float block_size;
    std::shared_ptr<MyRMan> R;

    public:
        /// @brief Подготавливает спрайт, текстуру и view для рисования мини-карты
        void prepare_minimap();

        /// @brief Обновляет изображение на мини-карте и возвращает ссылку на спрайт, готовый к отрисовке
        /// @return возвращает ссылку на спрайт, готовый к отрисовке
        sf::Sprite& draw_minimap(sf::Vector2f player_pos);
        miniMap(std::shared_ptr<MyRMan> r, const float bl_s) : R(r), block_size(bl_s){}
};

#endif