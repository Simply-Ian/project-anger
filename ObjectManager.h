#ifndef ANGER_OBJMAN
#define ANGER_OBJMAN
#include "GameObject.h"
#include "ResourceManager.h"
#include "Player.h"
#include "Camera.h"

struct ObjectManager{
    private:
        ResourceManager* R;
    public:
        ObjectManager(ResourceManager* r) : R(r){};
        Player player{R->lvl.player_x, R->lvl.player_y, R->player_speed};
        /// @todo Нефиксированное разрешение
        Camera cam{R->lvl, R->lvl.player_x, R->lvl.player_y, 1920};
};
#endif