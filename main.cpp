#include <memory>
#include "ResourceManager.h"
#include "WinManager.h"
#include "ObjectManager.h"

int main(int argc, char** argv){
    if (argc > 1){
        ResourceManager* const R = new ResourceManager();
        R->load_settings_file();
        R->load_level(argv[1]);

        ObjectManager* const O = new ObjectManager(R);
        WinManager wm{R, O};
        wm.loop();
    }
    return 0;
}