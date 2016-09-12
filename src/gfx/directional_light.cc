#include "gfx/directional_light.h"
#include "gfx/light.h"

gfx::DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 irradiance) :
    gfx::Light(irradiance), direction{direction} {}
