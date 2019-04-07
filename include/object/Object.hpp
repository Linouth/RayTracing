#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Vec3.hpp"
#include "utils/Color.hpp"

#include <cmath>

class Object {
   public:
    Object() = default;
    Object(const Color col) : color(col) {}
    virtual ~Object() = default;

    virtual Vec3 getNormal(const Vec3 &pi) = 0;
    virtual bool intersect(const Ray &ray, double &t) = 0;

    Color &getColor() { return color; };

   private:
    Color color;
};

class Sphere : public Object {
   public:
    Sphere(const Vec3 &center, const double radius, const Color &col)
        : Object(col), center(center), radius(radius) {}

    Vec3 getNormal(const Vec3 &pi) override { return (pi - center) / radius; }

    bool intersect(const Ray &ray, double &t) override {
        const Vec3 o = ray.o;
        const Vec3 d = ray.d;
        const Vec3 v = o - center;

        const double b = 2 * v.dot(d);
        const double c = v.dot(v) - radius * radius;
        double discriminant = b * b - 4 * c;

        if (discriminant <= 0)
            return false;

        discriminant = std::sqrt(discriminant);
        const double t0 = (-b + discriminant) / 2;
        const double t1 = (-b - discriminant) / 2;

        if (t0 < t1)
            t = t0;
        else
            t = t1;

        return true;
    }

    Vec3 &getCenter() {
        return center;
    }

   private:
    Vec3 center;
    double radius;
};

#endif
