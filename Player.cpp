#include "Player.h"
#include <cmath>
#include "conversions/rads_degs.h"

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

double Player::get_angle_degs(){
    double mod_rads = atan2(dir.y, dir.x);
    return to_degs(mod_rads);
}