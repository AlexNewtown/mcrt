#include "mcrt/scene.hh"

#include <glm/glm.hpp>
#include <limits>
#include <vector>
#include <cmath>

namespace mcrt {
    Ray::Intersection Scene::intersect(const Ray& ray) const {
        Ray::Intersection closestHit {
            std::numeric_limits<double>::max(),
                glm::dvec3(0.0),
                    {glm::dvec3(0.0),Material::Type::Diffuse,0.0},
                glm::dvec3(0.0)
                    };

        for (const Geometry* geometry : geometries) {
            Ray::Intersection rayHit = geometry->intersect(ray);
            if (rayHit.distance > 0.0 && rayHit.distance < closestHit.distance)
                closestHit = rayHit;
        }

        for (Light* lightSource : lights) {
            Ray::Intersection lightHit = lightSource->intersect(ray);
            if(lightHit.distance > 0.0 && lightHit.distance < closestHit.distance ){
                closestHit = lightHit;
            }
        }
        return closestHit;
    }

    double Scene::inShadow(const Ray& ray) const{

        double distance = std::numeric_limits<double>::max();

        for(const Geometry* geometry: geometries){
            Ray::Intersection rayHit = geometry->intersect(ray);

            if(rayHit.distance <= 0.0) {
                continue;
            }

            // Intersects Refractive surface
            if(rayHit.material.type == Material::Type::Refractive){
                continue;
            }
            distance = std::min(distance, rayHit.distance);
        }

        // Return distance to occlusion
        return distance;
    }

    
    // Will be our resource after this...
    void Scene::add(Geometry* geometry) {
        geometries.push_back(geometry);
    }

    void Scene::add(Light* light) {
        lights.push_back(light);
    }

    size_t Scene::maxRayDepth = 10;

    glm::dvec3 Scene::rayTrace(const Ray& ray, const int depth = 0) const {
        glm::dvec3 rayColor { 0.0 };

        // Make sure we don't bounce forever
        if(depth >= Scene::maxRayDepth)
            return rayColor;

        Ray::Intersection rayHit = intersect(ray);
        glm::dvec3 rayHitPosition { ray.origin + ray.direction * rayHit.distance };

        // Hit diffuse object
        if(rayHit.material.type == Material::Type::Diffuse) {
            for (Light* lightSource : lights) {
		        rayColor += lightSource->radiance(rayHit,this);	
            }
        }
        // Hit specular, mirror-like surface.
        else if(rayHit.material.type == Material::Type::Reflective) {
            Ray reflectionRay { ray.reflect(rayHitPosition, rayHit.normal) };
            rayColor += rayTrace(reflectionRay, depth + 1) * 0.9; // Falloff.
        }
        // Hit specular, transparent surface.
        else if(rayHit.material.type == Material::Type::Refractive) {
            double kr = ray.fresnel(rayHit.normal, rayHit.material.refractionIndex);
            bool outside = glm::dot(ray.direction, rayHit.normal) < 0.0;
            glm::dvec3 refractionColor = glm::dvec3(0.0);
            glm::dvec3 refractionDir;

            if(kr < 1.0) { // Check if ray isn't completely parallel to graze.
                Ray refractionRay { ray.refract(rayHitPosition, rayHit.normal,
                                                rayHit.material.refractionIndex) };
                refractionColor = rayTrace(refractionRay,depth + 1);
            }

            Ray reflectionRay; // If we need to invert the bias if we are inside.
            if (outside) reflectionRay = ray.reflect(rayHitPosition, rayHit.normal);
            else reflectionRay = ray.insideReflect(rayHitPosition, rayHit.normal);
            glm::dvec3 reflectionColor = rayTrace(reflectionRay, depth + 1);
            rayColor += reflectionColor * kr + refractionColor * (1.0 - kr);
        }
        // Hit light source
        else if(rayHit.material.type == Material::Type::LightSource) {
            //double cosa = glm::dot(ray.direction, rayOrigin.normal);
            double cosb = glm::dot(-ray.direction, rayHit.normal);
            if (cosb < 0.0) cosb = glm::dot(-ray.direction, -rayHit.normal);

            double geometricTerm = 1.0/rayHit.distance;
            rayColor += rayHit.material.color*geometricTerm;
        }

        return rayColor;
    }
}
