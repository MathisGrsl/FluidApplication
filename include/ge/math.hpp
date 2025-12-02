#include <cmath>

namespace ge {
    inline double abs(double nb)
    {
        if (nb < 0)
            return -nb;
        return nb;
    }
    inline double sqrt(double nb)
    {
        return std::sqrt(nb);
    }

    inline double pow(double nb, double exp)
    {
        return std::pow(nb, exp);
    }

    inline double pi()
    {
        return 3.14159265358979323846;
    }

    inline double deg2rad(double deg)
    {
        return deg * (pi() / 180.0);
    }

    inline double rad2deg(double rad)
    {
        return rad * (180.0 / pi());
    }
    inline double max(double x, double y)
    {
        if (x > y)
            return x;
        return y;
    }
    inline double min(double x, double y)
    {
        if (x < y)
            return x;
        return y;
    }
}
