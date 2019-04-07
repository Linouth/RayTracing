#ifndef COLOR_HPP
#define COLOR_HPP

#include <algorithm>
#include <ostream>

class Color {
    public:
        Color() = default;
        Color(double r, double g, double b) {
            this->r = (r > 255) ? 255 : (r < 0) ? 0 : r;
            this->g = (g > 255) ? 255 : (g < 0) ? 0 : g;
            this->b = (b > 255) ? 255 : (b < 0) ? 0 : b;
        }

        Color operator+(const Color &v) const {
            return Color((r + v.r)/2, (g + v.g)/2, (b + v.b)/2);
        }
        Color operator-(const Color &v) const { return Color(r - v.r, g - v.g, b - v.b); }
        Color operator*(const double d) const { return Color(r * d, g * d, b * d); }
        Color operator/(const double d) const { return Color(r / d, g / d, b / d); }

        // Color
        double r, g, b;
};

/*
std::ostream &operator<<(std::ostream &os, Color const &c) {
    return os << "r: " << c.r << ", g: " << c.g << ", b: " << c.b;
}
*/

#endif
