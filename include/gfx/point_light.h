// This class defines a point light represented by a position, attenuation values, and an
// irradiance.

// Brian Ho (brian@brkho.com)

#ifndef GFX_POINT_LIGHT_H
#define GFX_POINT_LIGHT_H

#include "gfx/light.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gfx {

class PointLight : public gfx::Light {
  public:
    // The position of the light.
    glm::vec3 position;

    // The constant attenuation coefficient of the light.
    GLfloat const_atten;

    // The linear attenuation coefficient of the light.
    GLfloat linear_atten;

    // The quadratic attenuation coefficient of the light.
    GLfloat quad_atten;

    // Constructor for a PointLight that specifies a color irradiance, position, constant
    // attenuation, linear attenuation, and quadratic attenuation.
    PointLight(glm::vec3 position, GLfloat const_atten, GLfloat linear_atten, GLfloat quad_atten,
        glm::vec3 irradiance);
};

}
#endif // GFX_POINT_LIGHT_H
