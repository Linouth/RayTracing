#include "Scene.hpp"

#include "utils/Color.hpp"
#include "object/Object.hpp"

#include <iostream>

Camera::Camera(int &width, int &height) {
    this->width = width;
    this->height = height;

    updateRayParameters();
}

void Camera::setTarget(const Vec3 &v) {
    T = v;
    updateRayParameters();
}

void Camera::setEye(const Vec3 &v) {
    E = v;
    updateRayParameters();
}

Ray Camera::getRay(const int x, const int y) const {
    // Calculate vector from Eye to pixel
    const Vec3 p_ij = p_1m + q_x * (x) + q_y * (y);

    // Normalize and return Ray
    return Ray(E, p_ij.normalize());
}

void Camera::updateRayParameters() {
    // Calculate image size in pixels (size/2)
    const double g_x = d * std::tan(theta / 2);
    const double g_y = g_x * height / width;

    // Define camera direction vectors
    const Vec3 t = T - E;  // target vector
    const Vec3 t_n = t.normalize();
    const Vec3 b_n = w.cross(t_n);    // horizontal vector
    const Vec3 v_n = t_n.cross(b_n);  // vertical vector

    // Calculate next pixel shifting vectors
    q_x = b_n * (2 * g_x / (width - 1));
    q_y = v_n * (2 * g_y / (height - 1));

    // Calculate position of bottom left pixel
    p_1m = t_n * d - b_n * g_x - v_n * g_y;
}

Scene::Scene(int width, int height) {
    this->width = width;
    this->height = height;

    camera = {width, height};
}

void Scene::initVideo() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("RayTracing", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
}

void Scene::run() {
    const Color white(255, 255, 255);
    const Color red(255, 0, 0);
    const Color black(0, 0, 0);

    Sphere light({0, 0, 50}, 1, white);

    std::vector<Object*> objects = {
        new Sphere({0, 0, 100}, 30, red),
        new Sphere({20, 5, 50}, 10, {0, 0, 255})
    };

    double t;
    Color pixel = black;

    bool quit = false;

    std::cout << "Starting loop\n";
    while (!quit) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                pixel = black;

                // Define ray from pixel vector
                Ray ray = camera.getRay(i, j);
                for (auto &object : objects) {
                    if (object->intersect(ray, t)) {
                        pixel = red;
                        const Vec3 pi = ray.o + ray.d * t;
                        const Vec3 L = light.getCenter() - pi;
                        const Vec3 N = object->getNormal(pi);
                        const double dt = L.normalize().dot(N.normalize());

                        pixel = object->getColor() + white * dt;
                    }

                    SDL_SetRenderDrawColor(renderer,
                                           (uint8_t)pixel.r,
                                           (uint8_t)pixel.g,
                                           (uint8_t)pixel.b,
                                           255);
                    SDL_RenderDrawPoint(renderer, i, j);
                }
            }
        }


        SDL_RenderPresent(renderer);

        const int STEPSIZE = 10;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_w:  // Light movement
                        light.getCenter().y -= STEPSIZE;
                        break;
                    case SDLK_a:
                        light.getCenter().x -= STEPSIZE;
                        break;
                    case SDLK_s:
                        light.getCenter().y += STEPSIZE;
                        break;
                    case SDLK_d:
                        light.getCenter().x += STEPSIZE;
                        break;
                    case SDLK_q:
                        light.getCenter().z += STEPSIZE;
                        break;
                    case SDLK_e:
                        light.getCenter().z -= STEPSIZE;
                        break;
                    case SDLK_l:
                        {
                        const Vec3 &c = light.getCenter();
                        std::cout << "x: " << c.x << ", y: " << c.y
                                  << ", z: " << c.z << std::endl;
                        break;
                        }

                    case SDLK_1:
                        camera.setEye({0, 0, 0});
                        camera.setTarget({0, 0, 1});
                        break;
                    case SDLK_2:
                        camera.setEye({0.1, 0, 0});
                        camera.setTarget({0, 0, 1});
                        break;
                    case SDLK_3:
                        camera.setEye({0.2, 0, 0});
                        camera.setTarget({0, 0, 1});
                        break;
                    case SDLK_4:
                        camera.setEye({0.3, 0, 0});
                        camera.setTarget({0, 0, 1});
                        break;
                    case SDLK_5:
                        camera.setEye({0.4, 0, 0});
                        camera.setTarget({0, 0, 1});
                        break;
                    case SDLK_6:
                        camera.setEye({1, 0, 1});
                        camera.setTarget({0.5, 0, 0});
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
