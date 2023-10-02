#include "Camera.h"
#include <cmath>
#include <algorithm>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <algorithm>

touchdownData Camera::get_touchdown_coords(double start_x, sf::Vector2f ray_coords, sf::Vector2f plane){

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
    const double delta = plane_width * (start_x - 0.5);
    const double dot_x = pos.x + (delta * dir.y);
    const double dot_y = pos.y - (delta * dir.x);

    // Луч идет строго вверх или вниз
    if (ray_coords.x == 0){
        if (ray_coords.y > 0){
            for (int line_y = ceil(dot_y); line_y <= level.size.y; line_y++){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                Block touched = level.get_block(floor(dot_x), line_y);
                if (!touched.is_a_null_block)
                    return touchdownData{sf::Vector2f{dot_x, line_y}, touchdownData::Side::Bottom, touched};
            }
        }
        // Луч идет строго вниз
        else {
            for (int line_y = floor(dot_y); line_y >= 0; line_y--){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                Block touched = level.get_block(floor(dot_x), line_y);
                if (!touched.is_a_null_block)
                    return touchdownData{sf::Vector2f{dot_x, line_y}, touchdownData::Side::Bottom, touched};
            }
        }
    }
    else if (ray_coords.y == 0){
        // Луч идет строго вправо
        if (ray_coords.x > 0){
            for (int line_x = ceil(dot_x); line_x <= level.size.x; line_x++){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                Block touched = level.get_block(line_x, floor(dot_y));
                if (!touched.is_a_null_block)
                    return touchdownData{sf::Vector2f{line_x, dot_y}, touchdownData::Side::Bottom, touched};
            }
        }
        // Луч идет строго влево
        else {
            for (int line_x = floor(dot_x); line_x >= 0; line_x--){
                // Проверяем, не принадлежит ли эта точка границе какого-то блока
                Block touched = level.get_block(line_x, floor(dot_y));
                if (!touched.is_a_null_block)
                    return touchdownData{sf::Vector2f{line_x, dot_y}, touchdownData::Side::Bottom, touched};
            }
        }
    }
    // Луч движется в I или IV четвертях
    else {
        int line_x = ray_coords.x > 0 ? ceil(dot_x) : floor(dot_x);
        // Точка ближайшего пересечения с вертикальной линией. Фиксированный X
        //Начальное значение y-координаты точки пересечения
        double init_y = with_vert_lines(line_x, dot_x, dot_y);
        touchdownData first_point = {{line_x, init_y}, ray_coords.x > 0? touchdownData::Side::Left : touchdownData::Side::Right,
                                    {}};
        bool first_point_valid = true;
        double new_y = init_y;
        Block touched;
        if (0 <= init_y && init_y < level.size.y){
            while(line_x >= 0 && line_x < level.size.x){
                try{
                    touched = level.get_block(ray_coords.x > 0? line_x : line_x - 1, floor(first_point.pos.y));
                    if (touched.is_a_null_block){
                        line_x += ray_coords.x > 0 ? 1 : -1;
                        first_point.pos.y = with_vert_lines(line_x, dot_x, dot_y);
                    }
                    else break;
                }
                catch (std::range_error){
                    first_point_valid = false;
                    break;
                }
            }
            first_point.pos.x = line_x;
            first_point.block = touched;
        }

        // Точка ближайшего пересечения с горизонтальной линией. Фиксированный Y
        bool second_point_valid = true;
        int line_y = ray_coords.y > 0 ? ceil(dot_y) : floor(dot_y);
        double init_x = with_hor_lines(line_y, dot_x, dot_y);
        touchdownData second_point = {{init_x, line_y}, ray_coords.y > 0? touchdownData::Side::Bottom : touchdownData::Side::Top,
                                    {}};
        if (0 <= init_x && init_x < level.size.x){
            while(0 <= line_y && line_y < level.size.y){
                try{
                    touched = level.get_block(floor(second_point.pos.x), ray_coords.y > 0? line_y : line_y - 1);
                    if (touched.is_a_null_block){
                        line_y += ray_coords.y > 0 ? 1 : -1;
                        second_point.pos.x = with_hor_lines(line_y, dot_x, dot_y);
                    }
                    else break;
                }
                catch (std::range_error){
                    second_point_valid = false;
                    break;
                }
            }
            second_point.pos.y = line_y;
            second_point.block = touched;
        }
        // Выбираем ближайшую точку
        if (!first_point_valid) return second_point;
        else if (!second_point_valid) return first_point;
        else return (pow(first_point.pos.x - dot_x, 2) + pow(first_point.pos.y - dot_y, 2)) <=
               (pow(second_point.pos.x - dot_x, 2) + pow(second_point.pos.y - dot_y, 2)) ? first_point : second_point;
    }
}

sf::RenderTexture* Camera::getImage(){
    auto multiply_color = [](sf::Color source, double factor) -> sf::Color {
        return {source.r * factor, source.g * factor,  source.b * factor};
    };

    const double glob_lighting_factors[] = {level.brightness, level.brightness * 0.5, level.brightness * 0.75, level.brightness * 0.75};

    sf::RenderTexture* image = new sf::RenderTexture;
    image->create(screen_res.x, screen_res.y);
    for (double offset = 0; offset < screen_res.x; offset++){
        double start_x = offset * (1.0 / screen_res.x);
        sf::Vector2f plane_vect = {dir.y, -dir.x};
        sf::Vector2f ray_coords = dir * static_cast<float>(distance_to_plane) + 
                                    plane_vect * (static_cast<float>((start_x - 0.5) * plane_width));

        touchdownData seen_point = get_touchdown_coords(start_x, ray_coords, plane_vect);
        
        double k = ray_coords.y / ray_coords.x;

        // Дистанция от позиции игрока до точки касания.
        double point_to_point = std::sqrt(std::pow(pos.x - seen_point.pos.x, 2) + std::pow(pos.y - seen_point.pos.y, 2));
        // Угол между плоскостью камеры и направлением от позиции игрока на точку (рад)
        double alpha = (atan2(seen_point.pos.y - pos.y, seen_point.pos.x - pos.x) - atan2(plane_vect.y, plane_vect.x));
        double dist_from_plane_to_point = std::sin(alpha) * point_to_point;

        // Второе слагаемое в знаменателе подобрано эмпирически
        // int raw_strip_height = screen_res.y * (distance_to_plane / (dist_from_plane_to_point + adjust));
        int raw_strip_height = screen_res.y  / dist_from_plane_to_point;
        int strip_height = std::min(raw_strip_height, static_cast<int>(screen_res.y));
        sf::RectangleShape drawableStrip{{1, strip_height}};

        /// @todo: загрузка значений из файла настроек
        double brightness = glob_lighting_factors[seen_point.side];
        brightness = std::max(brightness * (1 - point_to_point/level.decay_factor), 0.0);
        drawableStrip.setFillColor(multiply_color(seen_point.block.color, brightness));
        drawableStrip.setPosition(offset, (screen_res.y - strip_height) / 2);
        image->draw(drawableStrip);
    }
    image->display();
    return image;
}
