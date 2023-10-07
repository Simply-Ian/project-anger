#include <memory>
#include "Game.h"

int main(int argc, char** argv){
    if (argc > 1){
        Game g(argv[1]);
        g.start();
    }
    return 0;
}