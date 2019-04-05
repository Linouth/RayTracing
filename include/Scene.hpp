#ifndef SCENE_HPP
#define SCENE_HPP

#include "Vec3.hpp"

#include <SDL2/SDL.h>
#include <vector>

/* #include "Object.hpp" */

class Camera {
    public:
        /* Camera(int &width, int &height) : width(width), height(height) {} */
        Camera() = default;
        Camera(int &width, int &height);

        void setTarget(const Vec3 &v);
        void setEye(const Vec3 &v);

        Ray getRay(const int x, const int y) const;

    private:
        void updateRayParameters();

    private:
        int width, height;

        double d = 1.0;                 // Distance eye to image
        double theta = (3.141593 / 2);  // FOV
        Vec3 w = {0, 1, 0};

        Vec3 E = {0, 0, 0};  // Eye
        Vec3 T = {0, 0, 1};  // Target

        // Parameters to calculate Ray
        Vec3 p_1m, q_x, q_y;
};

class Scene {
    public:
        /* Scene(int width, int height) : camera{width, height}, width(width), height(height) {} */
        Scene(int width, int height);

        void initVideo();

        void run();

    private:
        /* std::vector<Object> objects; */

        SDL_Window *window;
        SDL_Renderer *renderer;

        Camera camera;

        int width, height;

};

#endif
