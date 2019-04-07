#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "Scene.hpp"

int main(void) {
    int W = 500;
    int H = 500;

    Scene scene = {W, H};
    scene.initVideo();

    scene.run();
}
