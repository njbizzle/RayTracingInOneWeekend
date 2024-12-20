#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"

#include "hittable.h"

class hittable_list : public hittable {
    public:

        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() {
            objects.empty();
        }

        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
        }


        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;

            // default the closet thing we have hit to the furthest we can see
            double closest_t = ray_t.max;

            for (const shared_ptr<hittable> object : objects) {
                if (object->hit(r, interval(ray_t.min, closest_t), temp_rec)) {
                    hit_anything = true;
                    closest_t = temp_rec.t;
                    rec = temp_rec; // this will be the rec of the last thing we hit
                }
            }

            return hit_anything;
        }
};

#endif
