#include "Camera.h"
#include <cmath>
#include <algorithm>

sf::Vector2f Camera::get_touchdown_coords(double start_x){
    sf::Vector2f plane = {dir.y, -dir.x};
    sf::Vector2f ray_coords = dir + plane * (static_cast<float>(start_x - 0.5));

    // start_x, start_y -- глобальные координаты точки, из которой выходит луч
    // line_x или line_y -- координата линии сетки, с которой ищется пересечение
    auto with_hor_lines = [&](int line_y, double start_x, double start_y) -> double {
        return start_x + (line_y - start_y) * (ray_coords.x / ray_coords.y);
    };

    auto with_vert_lines = [&](int line_x, double start_x, double start_y) -> double {
        return (ray_coords.y / ray_coords.x) * (line_x - start_x) + start_y;
    };

    // start_x -- смещение вдоль зрительной плоскости относительно ее левого конца. ЭТО НЕ ГЛОБАЛЬНАЯ КООРДИНАТА!
    // Вычисляем глобальные координаты точки, из которой выходит луч
    const double delta = start_x - (plane_width / 2);
    const double dot_x = pos.x + (delta * dir.y);
    const double dot_y = pos.y - (delta * dir.x);

    // Луч идет строго вверх или вниз
    if (ray_coords.x == 0){
        if (ray_coords.y > 0){
            for (int line_y = ceil(dot_y); line_y <= level.size.y; line_y++){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                if (!level.get_block(floor(dot_x), line_y).is_a_null_block)
                    return {dot_x, line_y};
            }
        }
        // Луч идет строго вниз
        else {
            for (int line_y = floor(dot_y); line_y >= 0; line_y--){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                if (!level.get_block(floor(dot_x), line_y).is_a_null_block)
                    return {dot_x, line_y};
            }
        }
    }
    else if (ray_coords.y == 0){
        // Луч идет строго вправо
        if (ray_coords.x > 0){
            for (int line_x = ceil(dot_x); line_x <= level.size.x; line_x++){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                if (!level.get_block(line_x, floor(dot_y)).is_a_null_block)
                    return {line_x, dot_y};
            }
        }
        // Луч идет строго влево
        else {
            for (int line_x = floor(dot_x); line_x >= 0; line_x--){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                if (!level.get_block(line_x, floor(dot_y)).is_a_null_block)
                    return {line_x, dot_y};
            }
        }
    }
    // Луч движется в I или IV четвертях
    else {
        int line_x = ray_coords.x > 0 ? ceil(dot_x) : floor(dot_x);
        // Точка ближайшего пересечения с вертикальной линией. Фиксированный X
        //Начальное значение y-координаты точки пересечения
        double init_y = with_vert_lines(line_x, dot_x, dot_y);
        sf::Vector2f first_point = {line_x, init_y};
        bool first_point_valid = true;
        double new_y = init_y;
        if (0 <= init_y && init_y < level.size.y){
            while(line_x >= 0 && line_x < level.size.x){
                try{
                    if (level.get_block(ray_coords.x > 0? line_x : line_x - 1, floor(first_point.y)).is_a_null_block){
                        line_x += ray_coords.x > 0 ? 1 : -1;
                        first_point.y = with_vert_lines(line_x, dot_x, dot_y);
                    }
                    else break;
                }
                catch (std::range_error){
                    first_point_valid = false;
                    break;
                }
            }
            first_point.x = line_x;
        }

        // Точка ближайшего пересечения с горизонтальной линией. Фиксированный Y
        sf::Vector2f second_point;
        bool second_point_valid = true;
        int line_y = ray_coords.y > 0 ? ceil(dot_y) : floor(dot_y);
        double init_x = with_hor_lines(line_y, dot_x, dot_y);
        second_point = sf::Vector2f{init_x, line_y};
        if (0 <= init_x && init_x < level.size.x){
            while(0 <= line_y && line_y < level.size.y){
                try{
                    if (level.get_block(floor(second_point.x), ray_coords.y > 0? line_y : line_y - 1).is_a_null_block){
                        line_y += ray_coords.y > 0 ? 1 : -1;
                        second_point.x = with_hor_lines(line_y, dot_x, dot_y);
                    }
                    else break;
                }
                catch (std::range_error){
                    second_point_valid = false;
                    break;
                }
            }
            second_point.y = line_y;
        }
        // Выбираем ближайшую точку
        if (!first_point_valid) return second_point;
        else if (!second_point_valid) return first_point;
        else return (pow(first_point.x - dot_x, 2) + pow(first_point.y - dot_y, 2)) <=
               (pow(second_point.x - dot_x, 2) + pow(second_point.y - dot_y, 2)) ? first_point : second_point;
    }
}