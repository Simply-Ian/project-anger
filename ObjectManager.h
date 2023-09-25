#ifndef ANGER_OBJMAN
#define ANGER_OBJMAN
#include "GameObject.h"
#include "ResourceManager.h"
#include "Player.h"

struct ObjectManager{
    private:
        ResourceManager* R;
    public:
        ObjectManager(ResourceManager* r) : R(r){};
        Player player{R->player_speed};
};
#endif