#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"

class camera {
    private:
        bool debug = false;

        double real_aspect_ratio;
        int image_height;

        double pixel_samples_scale;

        vec3 pixel_delta_u, pixel_delta_v;
        point3 camera_center, pixel_start;
        vec3 u, v, w; // camera basis vectors

        vec3 defocus_disk_u, defocus_disk_v;
        
        void initialize () {
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            real_aspect_ratio = double(image_width) / double(image_height);

            // camera
            double theta = degrees_to_radians(v_fov);
            double h = std::tan(theta/2); // ratio to tell us how tall our viewport must be (from the z axis)

            double viewport_height = 2.0 * h * focus_dist;
            double viewport_width = viewport_height * real_aspect_ratio;

            camera_center = look_from;

            // the first vector is simply the direction taking us straight towards the look at point
            w = unit_vector(look_from - look_at);

            // its a unit vector so we only need to take into account that the vector is perp to the others
            // by the right hand rule, index pointing up, middle pointing forward, this will be poiting left
            // relative to the camera
            u = unit_vector(cross(v_up, w));

            // v_up is relative to the camera and not the world which is why it is needed 
            // if v_up were relative to the world, it would be the same as this vector

            // following the right hand rule, index pointing forward, middle poiting left
            // this will point up 
            v = cross(w, u);

            // viewport
            vec3 viewport_u = viewport_width * u; // no longer using normal basis vectors
            vec3 viewport_v = viewport_height * -v; // flipping it because the raster scan does down

            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // focal length must be scaled by w
            point3 viewport_upper_left = camera_center -(focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel_start = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));

            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;

            // the camera (a point), forward is -z, up is +y, right is +x
            // the viewport (a plane in front of the camera), forward is -z, scans downwards so "up" is -y, right is +x
        }

        vec3 sample_square() const {
            // returns the vector to a random point in the [-0.5, -0.5]-[0.5, 0.5] unit square
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        ray get_ray(double i, double j) {
            vec3 offset = sample_square();

            point3 pixel_center = pixel_start + 
                ((i + offset.x()) * pixel_delta_u) + 
                ((j + offset.y()) * pixel_delta_v);
            
            // this makes certain points be in more focus then others
            // what this does is slightly offset the ray origin
            // while making sure that the ray still hits the percise location on the viewport
            // the slight change in the origin will have a growing effect as it goes futher from the focus plane
            // this is the same as how light diverges after being focused by a lens

            vec3 ray_origin = (defocus_angle >= 0) ? defocus_disk_sample() : camera_center;
            vec3 ray_direction = pixel_center - ray_origin;

            return ray(ray_origin, ray_direction);
        }

        color ray_color(const ray& r, int depth, const hittable& world) {
            if (depth <= 0) {
                return color(0, 0, 0);
            }

            hit_record rec;

            // floating point errors can put the point inside the sphere
            // in this case it could accidentally bounce off the surface its supposed to be flush with
            if (world.hit(r, interval(0.001, infinity), rec)) {

                if (debug)
                    std::clog << "hit | " << "r" << rec << '\n';

                ray scattered;
                color attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered))
                    return attenuation * ray_color(scattered, depth-1, world); // send another ray out
            } else {
                if (debug)
                    std::clog << "no hit" << '\n';
            }

            // sky
            vec3 unit_dir = unit_vector(r.direction());
            double a = 0.5 * (unit_dir.y() + 1.0);
            return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1);
        }

        point3 defocus_disk_sample() const {
            vec3 p = random_in_unit_disk();
            return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }


    public:
        double aspect_ratio = 1.0;
        double image_width = 100;
        
        int samples_per_pixel = 10;
        int max_depth = 10;

        double v_fov = 90;
        point3 look_from = point3(0,0,0), look_at = point3(0,0,-1);
        vec3 v_up = vec3(0,1,0); // up vector RELATIVE to the camera NOT the world

        double defocus_angle; // the max angle that the light can start diverging by after hitting the focus plane
        double focus_dist;

        void render(const hittable& world) {
            initialize();

            // needed to format the ppm
            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; j++) {

                // clog is similar to cout but normally used to log information
                // it its buffered similar to cout so std flush must be used 
                // \r moves the cursor to the front of the line, pretty much just overriding the line.

                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {

                    color pixel_color = color(0, 0, 0);

                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    pixel_color *= pixel_samples_scale;
                    write_color(std::cout, pixel_color);
                }
            }

            // extra space needed to write over the "scanlines remaining" text since we never break line and only use \r.
            std::clog << "\rDone.                    \n";
        }
        
        void debug_ray(int depth, const hittable& world) { debug_ray(image_width / 2, image_height / 2, depth, world); }

        void debug_ray(double i, double j, int depth, const hittable& world) {
            if (!debug)
                return;

            initialize();

            ray r = get_ray(i, j);
            color c = ray_color(r, depth, world);
            std::clog << "final color : " <<  c << '\n';
        }
};

#endif
