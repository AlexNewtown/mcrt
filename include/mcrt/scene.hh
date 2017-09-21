#ifndef MCRT_SCENE_HH
#define MCRT_SCENE_HH

#include "mcrt/ray.hh"
#include "mcrt/lights.hh"
#include "mcrt/camera.hh"
#include "mcrt/geometry.hh"

namespace mcrt {
    class Scene {
    public:
        Scene() = default;
        ~Scene() { for (auto g : geometries) delete g; }
        Scene& operator=(const Scene&) = delete;
        Scene(const Scene&) = delete;

        Scene(Scene&& other) noexcept {
            *this = std::move(other);
        }

        Scene& operator=(Scene&& other) noexcept {
            camera = other.camera;
            lights = other.lights;

            // Here comes the trick, rip this classes' guts out!
            for (size_t i { 0 }; i < other.geometries.size(); ++i) {
                geometries.push_back(other.geometries[i]);
                other.geometries[i] = nullptr;
            }

            return *this;
        }

        void add(const PointLight& light);
        void add(Geometry* geometry);

        glm::dvec3 rayTrace(const Ray& ray, const int) const;
        Ray::Intersection intersect(const Ray& ray) const;

        std::vector<Geometry*>& getGeometries() { return geometries; }
        const std::vector<Geometry*>& getGeometries() const { return geometries; }

        const std::vector<PointLight>& getPointLights() const { return lights; }
        std::vector<PointLight>& getPointLights() { return lights; }
        const Camera& getCamera() const { return camera; }
        Camera& getCamera() { return camera; }

    private:
        std::vector<Geometry*> geometries;
        std::vector<PointLight> lights;
        Camera camera;
    };
}

#endif
