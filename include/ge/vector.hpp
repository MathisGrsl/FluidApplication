#pragma once
#include <cmath>

namespace ge {

    struct v2 {
        float x, y;

        v2(float x = 0, float y = 0) : x(x), y(y) {}

        float length() const {
            return std::sqrt(x * x + y * y);
        }

        v2 normalized() const {
            float len = length();
            if (len == 0.0f) return {0, 0};
            return {x / len, y / len};
        }

        v2 operator+(const v2& o) const { return {x + o.x, y + o.y}; }
        v2 operator-(const v2& o) const { return {x - o.x, y - o.y}; }
        v2 operator*(float s) const { return {x * s, y * s}; }

        v2& operator+=(const v2& o) { x += o.x; y += o.y; return *this; }
        v2& operator-=(const v2& o) { x -= o.x; y -= o.y; return *this; }
    };

    struct v3 {
        float x, y, z;

        v3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        v3 normalized() const {
            float len = length();
            if (len == 0.0f) return {0, 0, 0};
            return {x / len, y / len, z / len};
        }

        v3 operator+(const v3& o) const { return {x + o.x, y + o.y, z + o.z}; }
        v3 operator-(const v3& o) const { return {x - o.x, y - o.y, z - o.z}; }
        v3 operator*(float s) const { return {x * s, y * s, z * s}; }

        v3& operator+=(const v3& o) { x += o.x; y += o.y; z += o.z; return *this; }
        v3& operator-=(const v3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    };

    struct v4 {
        float x, y, z, w;

        v4(float x = 0, float y = 0, float z = 0, float w = 0)
            : x(x), y(y), z(z), w(w) {}

        float length() const {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        v4 normalized() const {
            float len = length();
            if (len == 0.0f) return {0, 0, 0, 0};
            return {x / len, y / len, z / len, w / len};
        }

        v4 operator+(const v4& o) const { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
        v4 operator-(const v4& o) const { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
        v4 operator*(float s) const { return {x * s, y * s, z * s, w * s}; }

        v4& operator+=(const v4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
        v4& operator-=(const v4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
    };

    inline v2 operator*(float s, const v2& v) { return v * s; }
    inline v3 operator*(float s, const v3& v) { return v * s; }
    inline v4 operator*(float s, const v4& v) { return v * s; }

    inline v3 cross(const v3& a, const v3& b) {
        return v3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    inline float dot(const v2& a, const v2& b) {
        return a.x * b.x + a.y * b.y;
    }

    inline float dot(const v3& a, const v3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline float dot(const v4& a, const v4& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

}
