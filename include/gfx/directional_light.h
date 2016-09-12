// This class defines a directional light represented by a direction and an irradiance.

// Brian Ho (brian@brkho.com)

#ifndef GFX_DIRECTIONAL_LIGHT_H
#define GFX_DIRECTIONAL_LIGHT_H

#include "gfx/light.h"

#include <glm/glm.hpp>

namespace gfx {

class DirectionalLight : public gfx::Light {
  public:
    // The direction the light is coming from. This is not inverted!
    glm::vec3 direction;

    // Constructor for a DirectionalLight that specifies a color irradiance and direction.
    DirectionalLight(glm::vec3 direction, glm::vec3 irradiance);
};

}
#endif // GFX_DIRECTIONAL_LIGHT_H
