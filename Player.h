#ifndef ANGER_PLAYER
#define ANGER_PLAYER
#include "anger_engine/Camera.h"

class Player : public anger::Camera{
    public:
        /// @brief Функция, принимающая нажатые клавиши клавиатуры и двигающая игрока
        /// @param ws Принимает одно из трех значений: -1, 0, 1. -1 означает нажатую клавишу S, 1 -- W, 0 -- ни S, ни W не нажаты
        /// @param ad Принимает одно из трех значений: -1, 0, 1. -1 означает нажатую клавишу A, 1 -- D, 0 -- ни A, ни D не нажаты
        void move(int ws, int ad, double framelength);

        /// @brief Обёртка над GameObject::set_angle_degs(). Плавно поворачивает игрока в зависимости от положения курсора
        /// @param mouse_x X-координата курсора
        void set_angle_mouse(int mouse_x, int win_w_without_borders, int border);

        Player(double x, double y, double spd, const anger::Level& level, double plane_width, double d_t_p, 
                    sf::Vector2u screen_res) : anger::Camera(level, x, y, plane_width, d_t_p, screen_res){ 
            pos = {x, y};
            speed = spd;
        }
};
#endif