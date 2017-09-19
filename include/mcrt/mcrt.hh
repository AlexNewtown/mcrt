#ifndef MCRT_MCRT_HH
#define MCRT_MCRT_HH

#include <glm/glm.hpp>
#include <limits>
#include <vector>

namespace mcrt {

enum MaterialType {
    Diffuse     = 0,
    Reflective  = 1,
    Refractive  = 2
};

// Material struct, stores render information about a surface type.
struct Material {
    glm::dvec3 color;   
    MaterialType type;
    double ior;
};

struct Light {
    glm::dvec3 origin;
    glm::dvec3 color;
};

struct Ray {
    glm::dvec3 origin;
    glm::dvec3 direction;
};

struct Intersection {
    double distance;    // Distance to surface point
    glm::dvec3 normal;  // Normal of surface    
    Material material;
};

}
#endif
