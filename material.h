#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

#include "hittable.h"

class material {

    public:

        virtual ~material() = default;

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            return false;
        };

};

class lambertian : public material {
    private:
        color albedo;

    public:
        lambertian(const color& albedo) : albedo{albedo} {}

        bool scatter (
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 scatter_direction = rec.normal + random_unit_vector();

            // if the random unit vector is opposite the normal the dir is 0
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
            
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
};

class metal : public material {
    private:
        color albedo;
        double fuzz;
        
    public:
        metal(const color& albedo, double fuzz) : albedo{albedo}, fuzz{(fuzz > 1) ? 1 : fuzz} {}

        bool scatter (
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
         ) const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal);
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector()); // give a little random circle to fuzz the reflection
            scattered = ray(rec.p, reflected);

            attenuation = albedo;
            return true;
        }
};

class dielectric : public material {
  private:
    // the refractive index over the refractive index of the enclosing media
    double refractive_index;

    double reflectance(double cos, double ri) const {
        // https://en.wikipedia.org/wiki/Schlick%27s_approximation
        double r0 = (1 - ri) / (1 + ri);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cos), 5);
    }

  public:
    dielectric(double refractive_index) : refractive_index{refractive_index} {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        // if exiting the material then the refractive index is flipped
        double ri = rec.front_face ? (1.0 / refractive_index) : refractive_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
        
        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);

        attenuation = color(1.0, 1.0, 1.0);
        return true;
    }
    
};

#endif
