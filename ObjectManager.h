#ifndef ANGER_OBJMAN
#define ANGER_OBJMAN
#include "GameObject.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Camera.h"
#include "conversions/rads_degs.h"

struct ObjectManager{
    private:
        ResourceManager* R;
    public:
        ObjectManager(ResourceManager* r) : R(r){};
        Player player{R->lvl.player_x, R->lvl.player_y, R->player_speed};
        /// @todo Нефиксированное разрешение
        Camera cam{R->lvl, R->lvl.player_x, R->lvl.player_y, R->camera_plane_width, 
                0.5 / std::tan(to_rads(R->FOV / 2)) * R->camera_plane_width, {1920, 1080}};
};
#endif