#ifndef ANGER_CAMERA
#define ANGER_CAMERA
#include "GameObject.h"
#include <memory>
#include "ResourceManager.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

/// @brief Информация о точке касания лучом стены
struct touchdownData{
    sf::Vector2f pos;
    enum Side {Top, Bottom, Right, Left};
    Side side;
    Block block;
};

// /// @brief Представляет один столбец пикселей в изображении стены.
// struct imageRow{
//     sf::Color base_color;
//     double brightness;
//     int height;
// };

class Camera: public GameObject{
    double plane_width = 1;
    double distance_to_plane = 1.5;
    const ResourceManager::Level& level;
    sf::Vector2u screen_res;
    public:
        Camera(const ResourceManager::Level& lvl, double x, double y, sf::Vector2u s_r) : level(lvl), screen_res(s_r){
            pos = {x, y};
        }
        Camera(const ResourceManager::Level& lvl, double x, double y, double pw, double d_t_p, sf::Vector2u s_r) : 
                                    Camera(lvl, x, y, s_r){
            plane_width = pw;
            distance_to_plane = d_t_p;
        }

        /// @brief Метод для рендеринга изображения, видимого камерой
        /// @param single_height экранная высота стены высотой 1
        /// @return Готовое к отрисовке изображение
        sf::RenderTexture* getImage();

        // Публичная только на время отладки
        /// @brief Возвращает координаты первого столкновения луча со стеной
        /// @param start_x -- смещение по зрительной плоскости относительно ее левого конца (в долях от 1)
        touchdownData get_touchdown_coords(double start_x, sf::Vector2f ray_coords, sf::Vector2f plane);

        void setPos(sf::Vector2f new_pos){ 
            if ((pos.x >= 0 && pos.x <= level.size.x) && (pos.y >= 0 && pos.y <= level.size.y))
                pos = new_pos;
        }
};

#endif