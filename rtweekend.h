#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>


// usings

using std::make_shared;
using std::shared_ptr;

// constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// refractive indices
// https://en.wikipedia.org/wiki/List_of_refractive_indices
namespace refractive_indicies {
    const double air_ri = 1.000273;
    const double water_ri = 1.333;
    const double glass_ri = 1.458;
    const double diamond_ri = 2.417;
    const double amber_ri = 1.55;
}

// utility functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// headers

#include "interval.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"

#endif
