#include <iostream>

#include "mcrt/camera.hh"
#include "mcrt/image_export.hh"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

int main(int, char**) {
    mcrt::Image image { 16 };
    mcrt::Camera camera { { 0.0, 0.0, 0.0 }, {  0.0, 0.0, -1.0 } };
    std::cout << camera << std::endl;
    return 0;
}
