#ifndef ANGER_CAMERA
#define ANGER_CAMERA
#include "GameObject.h"
#include <memory>
#include "ResourceManager.h"
#include <SFML/Graphics/Color.hpp>

/// @brief Представляет один столбец пикселей в стене.
struct imageRow{
    sf::Color base_color;
    double brightness;
    int height;
};

class Camera: public GameObject{
    double plane_width = 1;
    double distance_to_plane = 1.5;
    const ResourceManager::Level& level;
    double screen_res_x;
    public:
        Camera(const ResourceManager::Level& lvl, double x, double y, double s_r) : level(lvl), screen_res_x(s_r){
            pos = {x, y};
        }
        Camera(const ResourceManager::Level& lvl, double x, double y, double pw, double d_t_p, double s_r) : 
                                    Camera(lvl, x, y, s_r){
            plane_width = pw;
            distance_to_plane = d_t_p;
        }
        std::unique_ptr<imageRow[]> get_image();

        // Публичная только на время отладки
        /// @brief Возвращает координаты первого столкновения луча со стеной
        /// @param start_x -- смещение по зрительной плоскости относительно ее левого конца (в долях от 1)
        sf::Vector2f get_touchdown_coords(double start_x);

        void setPos(sf::Vector2f new_pos){ 
            if ((pos.x >= 0 && pos.x <= level.size.x) && (pos.y >= 0 && pos.y <= level.size.y))
                pos = new_pos;
        }
};

#endif