#include "GameObject.h"
#include "conversions/rads_degs.h"
#include <cmath>
#include <algorithm>

sf::Vector2f GameObject::getPos(){
    return pos;
}

void GameObject::set_angle_rads(double angle){
    dir = {cos(angle), sin(angle)};
}

void GameObject::set_angle_degs(double angle){
    set_angle_rads(anger::to_rads(angle));
}

void GameObject::move(double framelength, sf::Vector2f vector){
    std::vector<Collision> collisions = get_collision();
    for (Collision col: collisions){
        if (col == Collision::TOP && vector.y > 0) vector.y = 0;
        else if (col == Collision::BOTTOM && vector.y < 0) vector.y = 0;

        if (col == Collision::RIGHT && vector.x > 0) vector.x = 0;
        else if (col == Collision::LEFT && vector.x < 0) vector.x = 0;
    }

    pos += vector * static_cast<float>((framelength / 1000) * speed);
}

double GameObject::get_angle_degs(){
    double mod_rads = atan2(dir.y, dir.x);
    return anger::to_degs(mod_rads);
}