#include "gfx/point_light.h"
#include "gfx/light.h"

gfx::PointLight::PointLight(glm::vec3 position, GLfloat const_atten, GLfloat linear_atten,
    GLfloat quad_atten, glm::vec3 irradiance) : gfx::Light(irradiance), position{position},
    const_atten{const_atten}, linear_atten{linear_atten}, quad_atten{quad_atten} {}
