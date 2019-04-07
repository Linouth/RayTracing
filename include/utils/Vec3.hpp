#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>
#include <ostream>


struct Vec3 {
    double x, y, z;

    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    Vec3() {}

    Vec3 operator+(const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(const double d) const { return Vec3(x * d, y * d, z * d); }
    Vec3 operator/(const double d) const { return Vec3(x / d, y / d, z / d); }

    double dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3 &v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    double magnitude2() const { return x * x + y * y + z * z; }
    double magnitude() const { return std::sqrt(magnitude2()); }
    Vec3 normalize() const {
        double mg = magnitude();
        return Vec3(x / mg, y / mg, z / mg);
    }

    double distance2(const Vec3 &to) const {
        return (to - *this).magnitude2();
    }
    double distance(const Vec3 &to) const {
        return (to - *this).magnitude();
    }
};

/* std::ostream &operator<<(std::ostream &os, Vec3 const &v) { */
/*     return os << "x: " << v.x << ", y: " << v.y << ", z: " << v.z; */
/* } */


struct Ray {
    Vec3 o;  // origin
    Vec3 d;  // Direction
    Ray(const Vec3 &o, const Vec3 &d) : o(o), d(d) {}
};

#endif
