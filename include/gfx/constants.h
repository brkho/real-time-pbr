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
// The number of samples to take for IBL. Note that this must match the number in main.frag and
// must be in base 2.
const unsigned int NUM_IBL_SAMPLES = 32;

}
#endif // GFX_CONSTANTS_H
