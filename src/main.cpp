#include <fstream>
#include <iostream>
#include <cmath>

struct Vec3 {
  double x, y, z;

  Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
  Vec3() {}

  Vec3 operator+(const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
  Vec3 operator-(const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
  Vec3 operator*(const double d) const { return Vec3(x * d, y * d, z * d); }
  Vec3 operator/(const double d) const { return Vec3(x / d, y / d, z / d); }

  double dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  double magnitude() const { return std::sqrt(x * x + y * y + z * z); }

  Vec3 normalize() const {
    double mg = magnitude();
    return Vec3(x / mg, y / mg, z / mg);
  }
};

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

int main(int argc, char *argv[]) {
    int W = 500;
    int H = 500;

    const Vec3 white(255, 255, 255);
    const Vec3 red(255, 0, 0);
    const Vec3 black(0, 0, 0);

    Sphere sphere({W*0.5, H*0.5, 50}, 50);
    Sphere light({0, 0, 50}, 1);

    std::ofstream out("out.ppm");
    out << "P3\n" << W << " " << H << " 255\n";

    double t;
    Vec3 pixel(black);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            pixel = black;

            Ray ray(Vec3(x, y, 0), Vec3(0, 0, 1));
            if (sphere.intersect(ray, t)) {
                pixel = red;
                const Vec3 pi = ray.o + ray.d*t;
                const Vec3 L = light.c - pi;
                const Vec3 N = sphere.getNormal(pi);
                const double dt = L.normalize().dot(N.normalize());

                pixel = (red + white*dt) * 0.5;
                tmp(pixel);
            }
            
            out << (int)pixel.x << " "
                << (int)pixel.y << " "
                << (int)pixel.z << "\n";
        }
    }
}
