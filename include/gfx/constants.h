// This header file defines constants used in the gfx module.

// Brian Ho (brian@brkho.com)

#ifndef GFX_CONSTANTS_H
#define GFX_CONSTANTS_H

#include "gfx/light.h"

#include <glm/glm.hpp>

namespace gfx {

// The maximum number of point lights in a scene.
const unsigned int MAX_POINT_LIGHTS = 3;
// The number of samples for MSAA.
const unsigned int MSAA_SAMPLES = 4;

}
#endif // GFX_CONSTANTS_H
