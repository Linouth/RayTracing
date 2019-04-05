#include <iostream>

#include <SDL2/SDL.h>

#include "Vec3.hpp"

struct Ray {
    Vec3 o;  // origin (?)
    Vec3 d;  // Direction
    Ray(const Vec3 &o, const Vec3 &d) : o(o), d(d) {}
};

struct Sphere {
    Vec3 c;
    double r;

    Sphere(const Vec3 &c, double r) : c(c), r(r) {}

    Vec3 getNormal(const Vec3 &p) {
        return (p - c) / r;
    }

    bool intersect(const Ray &ray, double &t) {
        const Vec3 o = ray.o;
        const Vec3 d = ray.d;
        const Vec3 v = o - c;

        const double b = 2 * v.dot(d);
        const double c = v.dot(v) - r*r;
        double discriminant = b*b - 4*c;

        if (discriminant <= 0)
            return false;

        discriminant = std::sqrt(discriminant);
        const double t0 = (-b + discriminant)/2;
        const double t1 = (-b - discriminant)/2;

        if (t0 < t1)
            t = t0;
        else
            t = t1;

        return true;
    }
};

void tmp(Vec3 &c) {
    c.x = (c.x > 255) ? 255 : (c.x < 0) ? 0 : c.x;
    c.y = (c.y > 255) ? 255 : (c.y < 0) ? 0 : c.y;
    c.z = (c.z > 255) ? 255 : (c.z < 0) ? 0 : c.z;
}

int main(void) {
    int W = 500;
    int H = 500;

    const Vec3 white(255, 255, 255);
    const Vec3 red(255, 0, 0);
    const Vec3 black(0, 0, 0);

    Sphere sphere({0, 0, 100}, 30);
    Sphere light({0, 0, 50}, 1);

    double ti;
    Vec3 pixel(black);

    // SDL Init Stuff
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("RayTracing", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                             SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // End SDL Stuff

    const double d = 1.0;
    const double theta = (3.141593/2);
    const Vec3 w = {0, 1, 0};

    Vec3 E = { 0, 0, 0 };
    /* Vec3 T = sphere.c; */
    Vec3 T = { 0, 0, 1};

    bool quit = false;
    while (!quit) {

        const double g_x = d * std::tan(theta/2);
        const double g_y = g_x * H/W;

        const Vec3 t = T - E;
        const Vec3 t_n = t.normalize();
        const Vec3 b_n = w.cross(t_n);
        const Vec3 v_n = t_n.cross(b_n);

        const Vec3 q_x = b_n * (2 * g_x / (W - 1));
        const Vec3 q_y = v_n * (2 * g_y / (H - 1));

        const Vec3 p_1m = t_n * d - b_n * g_x - v_n * g_y;

        for (int j = 0; j < H; j++) {
            for (int i = 0; i < W; i++) {
                pixel = black;
                const Vec3 p_ij = p_1m + q_x * (i) + q_y * (j);

                Ray ray(E, p_ij.normalize());

                /* Ray ray(Vec3(x, y, 100), Vec3(0, 0, -1)); */
                if (sphere.intersect(ray, ti)) {
                    pixel = red;
                    const Vec3 pi = ray.o + ray.d*ti;
                    const Vec3 L = light.c - pi;
                    const Vec3 N = sphere.getNormal(pi);
                    const double dt = L.normalize().dot(N.normalize());

                    pixel = (red + white*dt) * 0.5;
                    tmp(pixel);
                }

                SDL_SetRenderDrawColor(renderer, (uint8_t)pixel.x, (uint8_t)pixel.y, (uint8_t)pixel.z, 255);
                SDL_RenderDrawPoint(renderer, i, j);
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;

            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_q:
                        quit = true;
                        break;
                    case SDLK_w:
                        light.c.y -= 25;
                        break;
                    case SDLK_a:
                        light.c.x -= 25;
                        break;
                    case SDLK_s:
                        light.c.y += 25;
                        break;
                    case SDLK_d:
                        light.c.x += 25;
                        break;
                    case SDLK_e:
                        light.c.z += 25;
                        break;
                    case SDLK_r:
                        light.c.z -= 25;
                        break;
                    case SDLK_l:
                        std::cout << "x: " << light.c.x << ", y: " << light.c.y << ", z: " << light.c.z << std::endl;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
