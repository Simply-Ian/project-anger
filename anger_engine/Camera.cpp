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
    glob_lighting_factors[1] = level.brightness * 0.4;
    glob_lighting_factors[2] = level.brightness * 0.6;
    glob_lighting_factors[3] = level.brightness * 0.8;
}

Camera::Camera(const Level& lvl, double x, double y, double pw, double d_t_p, sf::Vector2u s_r) : 
                                    Camera(lvl, x, y, s_r){
    plane_width = pw;
    plane_height = screen_res.y * plane_width / screen_res.x;
    distance_to_plane = d_t_p;
}

double Camera::hor_intersect(int line_y, double start_x, double start_y, sf::Vector2f ray_coords) const {
    return start_x + (line_y - start_y) * (ray_coords.x / ray_coords.y);
}

double Camera::vert_intersect(int line_x, double start_x, double start_y, sf::Vector2f ray_coords) const {
    return (ray_coords.y / ray_coords.x) * (line_x - start_x) + start_y;
}

 sf::Vector2f Camera::get_start_point(double start_x){
    const double delta = plane_width * (start_x - 0.5);
    return {pos.x + (delta * dir.y), pos.y - (delta * dir.x)};
 }

touchdownData Camera::get_point_on_vert_line(sf::Vector2f ray_coords, double dot_x, double dot_y){
    int cur_x = ray_coords.x > 0 ? ceil(dot_x) : floor(dot_x);
    // Точка ближайшего пересечения с вертикальной линией. Фиксированный X
    //Начальное значение y-координаты точки пересечения
    double init_y = vert_intersect(cur_x, dot_x, dot_y, ray_coords);
    int x_step = ray_coords.x > 0 ? 1 : -1;
    double y_step = vert_intersect(cur_x + x_step, dot_x, dot_y, ray_coords) - init_y;
    touchdownData first_point = {{cur_x, init_y}, ray_coords.x > 0? anger::Block::Side::Left : anger::Block::Side::Right,
                                {}, true};
    double cur_y = init_y;
    Block touched;
    while (level.valid_coords({cur_x, cur_y})){
        touched = level.get_block(ray_coords.x > 0? cur_x : cur_x - 1, floor(cur_y));
        if (touched.is_a_null_block){
            cur_x += x_step;
            cur_y += y_step;
        }
        else {
            first_point.pos = {cur_x, cur_y};
            first_point.block = touched;
            return first_point;
        }
    }
    first_point.valid = false;
    return first_point;
}

touchdownData Camera::get_point_on_hor_line(sf::Vector2f ray_coords, double dot_x, double dot_y) {
    // Точка ближайшего пересечения с горизонтальной линией. Фиксированный Y
    int cur_y = ray_coords.y > 0 ? ceil(dot_y) : floor(dot_y);
    double init_x = hor_intersect(cur_y, dot_x, dot_y, ray_coords);
    int y_step = ray_coords.y > 0 ? 1 : -1;
    double x_step = hor_intersect(cur_y + y_step, dot_x, dot_y, ray_coords) - init_x;
    touchdownData second_point = {{init_x, cur_y}, ray_coords.y > 0? anger::Block::Side::Bottom : anger::Block::Side::Top,
                                {}, true};
    double cur_x = init_x;
    Block touched;
    while (level.valid_coords({cur_x, cur_y})){
        touched = level.get_block(floor(cur_x), ray_coords.y > 0? cur_y : cur_y - 1);
        if (touched.is_a_null_block){
            cur_x += x_step;
            cur_y += y_step;
        }
        else {
            second_point.pos = {cur_x, cur_y};
            second_point.block = touched;
            return second_point;
        }
    }
    second_point.valid = false;
    return second_point;
}

touchdownData Camera::get_touchdown_coords(sf::Vector2f start_dot, sf::Vector2f ray_coords, sf::Vector2f plane){
    touchdownData first_point;
    touchdownData second_point;
    Block touched;

    if (ray_coords.x != 0)
        first_point = get_point_on_vert_line(ray_coords, start_dot.x, start_dot.y);
    if (ray_coords.y != 0)
        second_point = get_point_on_hor_line(ray_coords, start_dot.x, start_dot.y);

    // Выбираем ближайшую точку
    if (!first_point.valid) return second_point;
    else if (!second_point.valid) return first_point;
    else return (pow(first_point.pos.x - start_dot.x, 2) + pow(first_point.pos.y - start_dot.y, 2)) <=
           (pow(second_point.pos.x - start_dot.x, 2) + pow(second_point.pos.y - start_dot.y, 2)) ? first_point : second_point;
}

sf::Color Camera::multiply_color(sf::Color source, double factor){
    return {std::min(source.r * factor, 255.0), std::min(source.g * factor, 255.0), std::min(source.b * factor, 255.0)};
}

void Camera::clear_buffer(){
    for (int x = 0; x < screen_res.x; x++)
        for (int y = 0; y < screen_res.y; y++)
            buffer.setPixel(x, y, {0, 0, 0, 0});
}

void Camera::calc_wall_strip(sf::Vector2f start_dot, sf::Vector2f ray_coords, sf::Vector2f plane_vect, std::shared_ptr<sf::Image>& skin,
                    int& raw_strip_height, double& brightness, int& texture_offset){
    touchdownData seen_point = get_touchdown_coords(start_dot, ray_coords, plane_vect);

    // Расстояние от точки на плоскости камеры до стены
    double dot_to_wall = std::sqrt(std::pow(start_dot.x - seen_point.pos.x, 2) + std::pow(start_dot.y - seen_point.pos.y, 2));

    // Расстояние от камеры до точки на плоскости
    double camera_to_dot = std::sqrt(std::pow(ray_coords.x, 2) + std::pow(ray_coords.y, 2));
    int texture_width;

    // Может принимать любые значения: хоть отрицательные, хоть большие вертикального разрешения экрана
    raw_strip_height = screen_res.y * (1 / plane_height) * (camera_to_dot / (camera_to_dot + dot_to_wall));
    brightness = glob_lighting_factors[seen_point.side];
    brightness = std::max(brightness * (1 - dot_to_wall/level.decay_factor), 0.0);

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
};  

void Camera::takeImage(){
    clear_buffer();

    double brightness;
    int raw_strip_height;
    double start_x;
    sf::Vector2f plane_vect = {dir.y, -dir.x};;
    sf::Vector2f ray_coords;
    sf::Vector2i strip_pos;
    std::shared_ptr<sf::Image> skin;
    int texture_offset; // Смещение столбца пикселей на исходной текстуре относительно левого края
    for (double offset = 0; offset < screen_res.x; offset++){
        start_x = offset * (1.0 / screen_res.x);
        ray_coords = dir * static_cast<float>(distance_to_plane) + 
                                    plane_vect * (static_cast<float>((start_x - 0.5) * plane_width));
        sf::Vector2f start_dot = get_start_point(start_x);

        calc_wall_strip(start_dot, ray_coords, plane_vect, skin, raw_strip_height, brightness, texture_offset);
        strip_pos = {offset, (static_cast<int>(screen_res.y) - raw_strip_height) / 2};
        draw_wall_strip(strip_pos, texture_offset, raw_strip_height, brightness, skin);
        if (strip_pos.y + raw_strip_height < screen_res.y)
            draw_floor_strip(start_dot, {strip_pos.x, strip_pos.y + raw_strip_height}, ray_coords);
    }
    shot.update(buffer);
}

void Camera::draw_wall_strip(sf::Vector2i strip_pos, int x_offset, double h, double brightness, std::shared_ptr<sf::Image> skin){
    // Y -- координата пикселя на текстуре-источнике
    int source_px_y;
    int buffer_h = buffer.getSize().y;
    size_t limit = std::min(static_cast<int>(h), buffer_h - strip_pos.y);
    int skin_height = skin->getSize().y;
    for (size_t adv = std::max(0, -strip_pos.y); adv < limit; adv++){
        source_px_y = std::floor(skin_height * adv / h);
        buffer.setPixel(strip_pos.x, strip_pos.y + adv, multiply_color(skin->getPixel(x_offset, source_px_y), brightness));
    }
}

bool Camera::is_px_shadowed(double glob_x, double glob_y){
    const double glob_light_angle_ctg = 0.25;
    
    // Координаты блока, отбрасывающего тень на текущий пиксел
    int shadow_block_y = std::ceil(glob_y);
    int shadow_block_x = std::floor(glob_x - glob_light_angle_ctg * (1 - (glob_y - std::floor(glob_y))));
    if (level.valid_coords({shadow_block_x, shadow_block_y}))
        if (!level.get_block(shadow_block_x, shadow_block_y).is_a_null_block)
            return true;
    
    shadow_block_y = std::floor(glob_y);
    shadow_block_x = std::floor(glob_x - glob_light_angle_ctg * (1 - (glob_y - std::floor(glob_y))));
    if (level.valid_coords({shadow_block_x, shadow_block_y}))
        if (!level.get_block(shadow_block_x, shadow_block_y).is_a_null_block)
            return true;

    shadow_block_y = std::ceil(glob_y);
    shadow_block_x = std::floor(glob_x - glob_light_angle_ctg);
    if (level.valid_coords({shadow_block_x, shadow_block_y}))
        if (!level.get_block(shadow_block_x, shadow_block_y).is_a_null_block)
            return true;
    return false;
}

void Camera::draw_floor_strip(sf::Vector2f start_dot, sf::Vector2i strip_pos, sf::Vector2f proj_coords){
    // Расстояние от камеры до текущего столбца на плоскости
    double d = std::sqrt(std::pow(proj_coords.x, 2) + std::pow(proj_coords.y, 2));
    int strip_h = screen_res.y - strip_pos.y;
    int px_y; // Y - координата текущего пиксела на экране.
    const double camera_z = 0.5; // Высота камеры над полом в блоках
    double D; // Расстояние от dot (точки на прямой, представл. плоскость камеры) до видимой точки пола. См. рисунок
    double px_glob_x; // ГЛОБАЛЬНЫЕ координаты видимой точки пола (в блоках)
    double px_glob_y;
    int texture_px_rel_x; // Координаты видимой точки пола относительно плитки
    int texture_px_rel_y;
    std::shared_ptr<sf::Image> tile_skin;
    double brightness;
    // Приводим вектор к длине = 1
    sf::Vector2f cast_coords;
    double angle = std::atan2(proj_coords.y, proj_coords.x); // Угол между лучем зрения и направлением глобальной оси Ox (рад)
    cast_coords.x = std::cos(angle);
    cast_coords.y = std::sin(angle);
    double h;

    for (size_t adv = 0; adv < strip_h; adv++){
        px_y = strip_pos.y + adv;
        h = camera_z - (strip_pos.y + adv - screen_res.y / 2.f) / screen_res.y * plane_height;
        D = d * h / (camera_z - h);
        brightness = std::max(level.brightness * (1 - D/level.decay_factor), 0.0);
        px_glob_x = D * cast_coords.x + start_dot.x; // ГЛОБАЛЬНЫЕ координаты
        px_glob_y = D * cast_coords.y + start_dot.y;

        tile_skin = level.get_tile(static_cast<int>(px_glob_x), static_cast<int>(px_glob_y));
        if (tile_skin != nullptr){
            texture_px_rel_x = tile_skin->getSize().x * (px_glob_x - std::floor(px_glob_x));
            texture_px_rel_y = tile_skin->getSize().y * (px_glob_y - std::floor(px_glob_y));
            buffer.setPixel(strip_pos.x, px_y, multiply_color(tile_skin->getPixel(texture_px_rel_x, texture_px_rel_y), 
                brightness * (is_px_shadowed(px_glob_x, px_glob_y)? 0.45 : 1)));
        }
    }
}