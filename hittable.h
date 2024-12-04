#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"

// forward delcleration to avoid circular import
class material;

class hit_record {

    public:
        point3 p;
        vec3 normal;
        shared_ptr<material> mat; 
        // shared pointer because there is no instance 
        // specific data important to the hit record 
        double t;
        bool front_face;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // normals must have unit length
            
            front_face = dot(r.direction(), outward_normal) < 0; 
            // the ray and the outer normal must be pointing opposite directions to be a front hit
            // dotting the two vectors tells us how they are oriented relative to eachother
            normal = front_face ? outward_normal : -outward_normal; // flip the normal if needed
        }
        friend std::ostream& operator<<(std::ostream& out, const hit_record& hit);
};

std::ostream& operator<<(std::ostream& out, const hit_record& hit) {
    return out << "hit record :  \n   [material : " << " " << 
        "], \n   [point : " << hit.p << 
        "], \n   [normal : " << hit.normal << 
        "], \n   [front face : " << hit.front_face << 
        "]\n";
}


class hittable {
    public:
        virtual ~hittable() = default;
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif
