#include "Player.h"
#include <cmath>
#include "anger_engine/conversions/rads_degs.h"

void Player::move(int ws, int ad, double framelength){
    if (ws && ad){
        GameObject::move(framelength, dir * static_cast<float>(ws * M_SQRT2 / 2.f));
        GameObject::move(framelength, sf::Vector2f{dir.y, -dir.x} * static_cast<float>(ad * M_SQRT2 / 2.f));
    }
    else if (ws)
        GameObject::move(framelength, dir * static_cast<float>(ws));
    else if (ad)
        GameObject::move(framelength, sf::Vector2f{dir.y, -dir.x} * static_cast<float>(ad));
}

void Player::set_angle_mouse(int mouse_x, int win_w_without_borders, int border){
    set_angle_degs(180*(win_w_without_borders - 2*(mouse_x - 2*border)) / win_w_without_borders);
}

std::vector<GameObject::Collision> Player::get_collision() const{
    int floor_x = static_cast<int>(pos.x);
    int floor_y = static_cast<int>(pos.y);
    double d_x = pos.x - floor_x; // Дробная часть X-координаты
    double d_y = pos.y - floor_y; // Дробная часть Y-координаты
    std::vector<Collision> to_return;

    if (d_x <= collide_box.width){
        if (!level.get_block(floor_x - 1, floor_y).is_a_null_block)
            to_return.push_back(Collision::LEFT);
    }
    else if (1 - d_x <= collide_box.width){
        if (!level.get_block(floor_x + 1, floor_y).is_a_null_block)
            to_return.push_back(Collision::RIGHT);
    }
    if (d_y <= collide_box.width){
        if (!level.get_block(floor_x, floor_y - 1).is_a_null_block)
            to_return.push_back(Collision::BOTTOM);
    }
    else if (1 - d_y <= collide_box.width){
        if (!level.get_block(floor_x, floor_y + 1).is_a_null_block)
            to_return.push_back(Collision::TOP);
    }

    if (to_return.empty()) return {Collision::NONE};
    else return to_return;
}