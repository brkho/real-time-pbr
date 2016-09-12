// This interface defines the base properties of a light.

// Brian Ho (brian@brkho.com)

#ifndef GFX_LIGHT_H
#define GFX_LIGHT_H

#include "glm/glm.hpp"

namespace gfx {

class Light {
  public:
    // The irradiance of the light in RGB. Note that this is not a color because in physically
    // based shading, the irradiance can exceed 1.0.
    glm::vec3 irradiance;

  protected:
    // Protected initializer for intensity.
    Light(glm::vec3 irradiance) : irradiance{irradiance} {}
};

}
#endif // GFX_LIGHT_H
