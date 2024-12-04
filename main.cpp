#include <iostream>

#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

int main() {
    
    hittable_list world;

    using namespace refractive_indicies;

    shared_ptr<material> material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    shared_ptr<material> material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    shared_ptr<material> material_left = make_shared<dielectric>(glass_ri / air_ri);
    shared_ptr<material> material_bubble = make_shared<dielectric>(air_ri / glass_ri);
    shared_ptr<material> material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400; // 1920 for "full res", 400 as a good starting point

    cam.samples_per_pixel = 100; // 100
    cam.max_depth = 50; // 50

    cam.v_fov = 30;
    
    double dist_to_center_orb = (point3(0.0, 0.0, -1.2) - point3(-2, 1, 1.5)).length();
    // std::clog << dist_to_center_orb << '\n';

    cam.look_from = point3(-2, 1, 1.5);
    cam.look_at = point3(-0.25, 0, -1);
    cam.v_up = vec3(0, 1, 0);

    cam.defocus_angle = 3.0;
    cam.focus_dist = dist_to_center_orb;

    cam.render(world);
}
