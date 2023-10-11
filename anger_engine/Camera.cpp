#include "Camera.h"
#include <cmath>
#include <algorithm>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>
using namespace anger;

Camera::Camera(const Level& lvl, double x, double y, sf::Vector2u s_r) : level(lvl), screen_res(s_r){
    pos = {x, y};
    // cur_image.create(screen_res.x, screen_res.y);
    // cur_image.setSmooth(true);
    shot.create(screen_res.x, screen_res.y);
    buffer.create(screen_res.x, screen_res.y, sf::Color::Black);
    glob_lighting_factors[0] = level.brightness;
    glob_lighting_factors[1] = level.brightness * 0.5;
    glob_lighting_factors[2] = level.brightness * 0.75;
    glob_lighting_factors[3] = level.brightness * 0.75;
}

Camera::Camera(const Level& lvl, double x, double y, double pw, double d_t_p, sf::Vector2u s_r) : 
                                    Camera(lvl, x, y, s_r){
    plane_width = pw;
    distance_to_plane = d_t_p;
}

double Camera::hor_intersect(int line_y, double start_x, double start_y, sf::Vector2f ray_coords) const {
    return start_x + (line_y - start_y) * (ray_coords.x / ray_coords.y);
}

double Camera::vert_intersect(int line_x, double start_x, double start_y, sf::Vector2f ray_coords) const {
    return (ray_coords.y / ray_coords.x) * (line_x - start_x) + start_y;
}

touchdownData Camera::get_point_on_vert_line(sf::Vector2f ray_coords, double dot_x, double dot_y){
    int line_x = ray_coords.x > 0 ? ceil(dot_x) : floor(dot_x);
    // Точка ближайшего пересечения с вертикальной линией. Фиксированный X
    //Начальное значение y-координаты точки пересечения
    double init_y = vert_intersect(line_x, dot_x, dot_y, ray_coords);
    touchdownData first_point = {{line_x, init_y}, ray_coords.x > 0? anger::Block::Side::Left : anger::Block::Side::Right,
                                {}};
    double new_y = init_y;
    Block touched;
    if (0 <= init_y && init_y < level.size.y){
        first_point.valid = true;
        while(line_x >= 0 && line_x < level.size.x){
            try{
                touched = level.get_block(ray_coords.x > 0? line_x : line_x - 1, floor(first_point.pos.y));
                if (touched.is_a_null_block){
                    line_x += ray_coords.x > 0 ? 1 : -1;
                    first_point.pos.y = vert_intersect(line_x, dot_x, dot_y, ray_coords);
                }
                else break;
            }
            catch (std::range_error){
                first_point.valid = false;
                break;
            }
        }
        first_point.pos.x = line_x;
        first_point.block = touched;
    }
    return first_point;
}

touchdownData Camera::get_point_on_hor_line(sf::Vector2f ray_coords, double dot_x, double dot_y) {
    // Точка ближайшего пересечения с горизонтальной линией. Фиксированный Y
    int line_y = ray_coords.y > 0 ? ceil(dot_y) : floor(dot_y);
    double init_x = hor_intersect(line_y, dot_x, dot_y, ray_coords);
    touchdownData second_point = {{init_x, line_y}, ray_coords.y > 0? anger::Block::Side::Bottom : anger::Block::Side::Top,
                                {}, false};
    Block touched;
    if (0 <= init_x && init_x < level.size.x){
        second_point.valid = true;
        while(0 <= line_y && line_y < level.size.y){
            try{
                touched = level.get_block(floor(second_point.pos.x), ray_coords.y > 0? line_y : line_y - 1);
                if (touched.is_a_null_block){
                    line_y += ray_coords.y > 0 ? 1 : -1;
                    second_point.pos.x = hor_intersect(line_y, dot_x, dot_y, ray_coords);
                }
                else break;
            }
            catch (std::range_error){
                second_point.valid = false;
                break;
            }
        }
        second_point.pos.y = line_y;
        second_point.block = touched;
    }
    return second_point;
}

touchdownData Camera::get_touchdown_coords(double start_x, sf::Vector2f ray_coords, sf::Vector2f plane){
    // start_x -- смещение вдоль зрительной плоскости относительно ее левого конца. ЭТО НЕ ГЛОБАЛЬНАЯ КООРДИНАТА!
    // Вычисляем глобальные координаты точки, из которой выходит луч
    const double delta = plane_width * (start_x - 0.5);
    const double dot_x = pos.x + (delta * dir.y);
    const double dot_y = pos.y - (delta * dir.x);
    touchdownData first_point;
    touchdownData second_point;
    Block touched;

    if (ray_coords.x != 0)
        first_point = get_point_on_vert_line(ray_coords, dot_x, dot_y);
    if (ray_coords.y != 0)
        second_point = get_point_on_hor_line(ray_coords, dot_x, dot_y);

    // Выбираем ближайшую точку
    if (!first_point.valid) return second_point;
    else if (!second_point.valid) return first_point;
    else return (pow(first_point.pos.x - dot_x, 2) + pow(first_point.pos.y - dot_y, 2)) <=
           (pow(second_point.pos.x - dot_x, 2) + pow(second_point.pos.y - dot_y, 2)) ? first_point : second_point;
}

double Camera::calculate_dist_plane_to_point(sf::Vector2f point_pos, sf::Vector2f plane_vect){
    // Дистанция от позиции игрока до точки касания.
    double point_to_point = std::sqrt(std::pow(pos.x - point_pos.x, 2) + std::pow(pos.y - point_pos.y, 2));
    // Угол между плоскостью камеры и направлением от позиции игрока на точку (рад)
    double alpha = (atan2(point_pos.y - pos.y, point_pos.x - pos.x) - atan2(plane_vect.y, plane_vect.x));
    return std::sin(alpha) * point_to_point;
}

sf::Color Camera::multiply_color(sf::Color source, double factor){
    return {std::min(source.r * factor, 255.0), std::min(source.g * factor, 255.0), std::min(source.b * factor, 255.0)};
}

void Camera::clear_buffer(){
    for (int x = 0; x < screen_res.x; x++)
        for (int y = 0; y < screen_res.y; y++)
            buffer.setPixel(x, y, {0, 0, 0, 0});
}

void Camera::takeImage(){
    clear_buffer();

    double plane_to_point;
    double brightness;
    int raw_strip_height;
    // int strip_height;
    // sf::RectangleShape drawableStrip{{1, 0}};
    double start_x;
    for (double offset = 0; offset < screen_res.x; offset++){
        start_x = offset * (1.0 / screen_res.x);
        sf::Vector2f plane_vect = {dir.y, -dir.x};
        sf::Vector2f ray_coords = dir * static_cast<float>(distance_to_plane) + 
                                    plane_vect * (static_cast<float>((start_x - 0.5) * plane_width));

        touchdownData seen_point = get_touchdown_coords(start_x, ray_coords, plane_vect);

        plane_to_point = calculate_dist_plane_to_point(seen_point.pos, plane_vect);
        raw_strip_height = screen_res.y  / plane_to_point;
        // strip_height = std::min(raw_strip_height, static_cast<int>(screen_res.y));
        brightness = glob_lighting_factors[seen_point.side];
        brightness = std::max(brightness * (1 - plane_to_point/level.decay_factor), 0.0);

        // Если блок просто закрашен цветом и не имеет текстур
        if (seen_point.block.t_right == nullptr){
            // drawableStrip.setSize({1, strip_height});
            // drawableStrip.setFillColor(multiply_color(seen_point.block.color, brightness));
            // drawableStrip.setPosition(offset, (screen_res.y - strip_height) / 2);
            // cur_image.draw(drawableStrip);
        }
        // Блок затекстурирован
        else {
            int texture_offset;
            int texture_width;
            std::shared_ptr<sf::Image> skin;

            if (seen_point.side == anger::Block::Side::Top){
                texture_width = seen_point.block.t_top->getSize().x;
                texture_offset = texture_width * (1 - (seen_point.pos.x - std::floor(seen_point.pos.x)));
                skin = seen_point.block.t_top;
            }
            else if (seen_point.side == anger::Block::Side::Bottom){
                texture_width = seen_point.block.t_bottom->getSize().x;
                texture_offset = texture_width * (seen_point.pos.x - std::floor(seen_point.pos.x));
                skin = seen_point.block.t_bottom;
            }
            else if (seen_point.side == anger::Block::Side::Left){
                texture_width = seen_point.block.t_left->getSize().x;
                texture_offset = texture_width * (seen_point.pos.y - std::floor(seen_point.pos.y));
                skin = seen_point.block.t_left;
            }
            else if (seen_point.side == anger::Block::Side::Right){
                texture_width = seen_point.block.t_right->getSize().x;
                texture_offset = texture_width * (1 - (seen_point.pos.y - std::floor(seen_point.pos.y)));
                skin = seen_point.block.t_right;
            }
            draw_wall_strip({offset, (static_cast<int>(screen_res.y) - raw_strip_height) / 2}, texture_offset, raw_strip_height, brightness, skin);
            // strip.setPosition(offset, (screen_res.y - strip.getGlobalBounds().height) / 2);
            // cur_image.draw(strip);
        }
    }
    shot.update(buffer);
    
    // cur_image.display();
    // shot = cur_image.getTexture();
}

void Camera::draw_wall_strip(sf::Vector2i pos, int x_offset, int h, double brightness, std::shared_ptr<sf::Image> skin){
    // Y -- координата пикселя на текстуре-источнике
    int source_px_y;
    int buffer_h = buffer.getSize().y;
    for (size_t adv = std::max(0, -pos.y); adv < std::min(h, buffer_h - pos.y); adv++){
        // if (pos.y + adv >= 0 && pos.y + adv < buffer.getSize().y){
            source_px_y = std::floor(static_cast<double>(skin->getSize().y) * adv / h);
            buffer.setPixel(pos.x, pos.y + adv, multiply_color(skin->getPixel(x_offset, source_px_y), brightness));
        // }
    }
}