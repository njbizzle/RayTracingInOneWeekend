#ifndef RAY_H
#define RAY_H

#include "rtweekend.h"

class ray {

    private:
        point3 orig;
        vec3 dir;

    public:
        ray() {}
        ray(const point3& origin, const vec3& direction) : orig {origin}, dir {direction} {}

        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        point3 at(double t) const {
            return orig + (t * dir);
        }

        friend std::ostream& operator<<(std::ostream& out, const ray& r);
};


std::ostream& operator<<(std::ostream& out, const ray& r) {
    return out << "ray :  \n   [origin : " << r.origin() << 
        "], \n   [dir : " << r.direction() << 
        "]\n";
}


#endif
