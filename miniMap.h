#ifndef MINIMAP_PROJANGER
#define MINIMAP_PROJANGER
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include "MyRMan.h"
#include <memory>

class miniMap{
    sf::Sprite mapSprite;
    sf::RenderTexture texture;
    sf::View mapView;
    /// @brief Зависимость draw_minimap(). Рисует на мини-карте стены.
    void draw_blocks();

    /// @brief Зависимость draw_minimap(). Рисует на мини-карте игрока
    void draw_player(float player_screen_x, float player_screen_y);

    /// @brief Зависимость draw_minimap(). Рисует на мини-карте метку (0, 0)
    void draw_zero_mark();

    int inverse_y(int it_y);
    float block_size;
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