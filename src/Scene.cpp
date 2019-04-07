#include "Scene.hpp"

#include "utils/Color.hpp"
#include "object/Object.hpp"

#include <set>
#include <iostream>
#include <algorithm>

Camera::Camera(int &width, int &height) {
    this->width = width;
    this->height = height;

    updateRayParameters();
}

Vec3 Camera::getPos() const {
    return E;
}

void Camera::setTarget(const Vec3 &v) {
    T = v;
    updateRayParameters();
}

void Camera::lookAt(const Vec3 &v) {
    setTarget((v - E));
}

void Camera::setEye(const Vec3 &v) {
    E = v;
    updateRayParameters();
}

void Camera::moveEye(const Vec3 &v) {
    setEye(E + v);
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
    const Vec3 b_n = t_n.cross(w);    // horizontal vector
    const Vec3 v_n = b_n.cross(t_n);  // vertical vector

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

Scene::~Scene() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    for (auto object: objects)
        delete object;
    objects.clear();

    for (auto light: lights)
        delete light;
    lights.clear();
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

    lights = {
        new Sphere({0, 0, -50}, 1, white)
    };

    objects = {
        new Sphere({0, 0, -100}, 30, red),
        new Sphere({20, 5, -150}, 15, {0, 0, 255})
    };


    double t;
    Color pixel = black;

    bool animation = false;
    double x, z;
    double ts = 0;

    bool quit = false;
    while (!quit) {
        // set timeout to limit frame rate
        Uint32 timeout = SDL_GetTicks() + 20;

        // TODO: Temporary camera distance update
        for (auto &o : objects)
            o->update(camera.getPos());
        // Sort objects on distance from camera
        std::sort(objects.begin(), objects.end(), Object::compByDistance);

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                pixel = black;

                // Define ray from pixel vector
                Ray ray = camera.getRay(i, j);
                for (auto &object : objects) {
                    if (object->intersect(ray, t)) {
                        // Camera ray intersects with object
                        const Vec3 pi = ray.o + ray.d * t;
                        const Vec3 N = object->getNormal(pi);
                        double dt = 0;

                        for (auto &light : lights) {
                            // Define ray from pi to light
                            Ray lightRay = {
                                pi,
                                (light->getCenter() - pi).normalize()
                            };

                            for (auto &o : objects) {
                                if (o != object
                                    && o->intersect(lightRay, t)
                                    && t > 0) {
                                    dt = 0;
                                    break;
                                }
                                // Didn't hit object,
                                // clear path to light
                                dt = lightRay.d.dot(N.normalize());
                            }
                        }

                        pixel = object->getColor() + white * dt;

                        // break from loop so only first intersect is drawn
                        break;
                    }
                }
                // Draw pixel to the screen
                SDL_SetRenderDrawColor(renderer,
                                       (uint8_t)pixel.r,
                                       (uint8_t)pixel.g,
                                       (uint8_t)pixel.b,
                                       255);
                // Note: y=0 is at top, height-y starts at bottom
                // and goes to the top of the screen
                SDL_RenderDrawPoint(renderer, i, height-1-j);
            }
        }


        SDL_RenderPresent(renderer);

        const int STEPSIZE = 10;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;

            Object *light = lights.back();
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_w:  // Light movement
                        light->getCenter().y -= STEPSIZE;
                        break;
                    case SDLK_a:
                        light->getCenter().x -= STEPSIZE;
                        break;
                    case SDLK_s:
                        light->getCenter().y += STEPSIZE;
                        break;
                    case SDLK_d:
                        light->getCenter().x += STEPSIZE;
                        break;
                    case SDLK_q:
                        light->getCenter().z += STEPSIZE;
                        break;
                    case SDLK_e:
                        light->getCenter().z -= STEPSIZE;
                        break;
                    case SDLK_p:
                        {
                        const Vec3 &c = light->getCenter();
                        std::cout << "x: " << c.x << ", y: " << c.y
                                  << ", z: " << c.z << std::endl;
                        break;
                        }

                    case SDLK_j:
                        camera.moveEye({-1, 0, 0});
                        camera.lookAt(light->getCenter());
                        break;
                    case SDLK_l:
                        camera.moveEye({1, 0, 0});
                        camera.lookAt(light->getCenter());
                        break;

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

                    case SDLK_z:
                        animation = !animation;
                        break;
                    default:
                        break;
                }
            }
        }

        if (animation) {
            const double centerOfRotation = 137.5;

            x = centerOfRotation * (std::sin(3.1416/4 * ts));
            z = centerOfRotation * (-std::cos(3.1416/4 * ts) + 1);
            ts += 0.1;

            camera.setEye({x, 0, -z});
            camera.setTarget({0, 0, -centerOfRotation});
        }

        while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout)) {}
    }

    std::cout << "Closing\n";
}
