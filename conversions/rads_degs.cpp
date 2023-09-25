#include <cmath>

double to_rads(double degs){
    return std::fmod(degs, 360) / 180 * M_PI;
}

double to_degs(double rads){
    return std::fmod(rads, 2 * M_PI) / M_PI * 180;
}