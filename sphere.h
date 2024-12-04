#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"

#include "hittable.h"

class sphere : public hittable {

    private:
        point3 center;
        double radius;
        shared_ptr<material> mat;

    public:
        sphere(const point3& center, double radius, shared_ptr<material> mat) 
        : center{center}, radius{std::fmax(0,radius)}, mat{mat} {

        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

            // return radius * radius >=
            //     (center.x() - r.direction().x()) * (center.x() - r.direction().x()) + 
            //     (center.y() - r.direction().y()) * (center.y() - r.direction().y()) + 
            //     (center.y() - r.direction().z()) * (center.z() - r.direction().z());

            // dot product of a vector is the magnitude squared. (projecting on to itself is itself)
            // saying the magnitude squared is in a range, that is the definition of a sphere / circle

            // turns into a quadratic solving for the time where it hits
            // if discriminant is - there are no hits, 0 means tangent, + means 2 hits.

            vec3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = h*h - a*c;

            if (discriminant < 0) { // if no hits
                return false;
            }

            double sqrtd = std::sqrt(discriminant);
            double hit_t = (h - sqrtd) / a;

            if (!ray_t.surrounds(hit_t)) { // if the time is out of the range
                hit_t = (h + sqrtd) / a;
                if (!ray_t.surrounds(hit_t)) // try the other solution
                    return false; // then its out of range
            }

            rec.t = hit_t;
            rec.p = r.at(hit_t);
            vec3 outward_normal = (rec.p - center) / radius; 
            rec.set_face_normal(r, outward_normal);
            // normal vector is the one from the center to the hit
            // it will have a length of the radius, so rescale it
            rec.mat = mat;

            return true;
        }
};

#endif
