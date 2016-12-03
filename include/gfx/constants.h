// This header file defines constants used in the gfx module.

// Brian Ho (brian@brkho.com)

#ifndef GFX_CONSTANTS_H
#define GFX_CONSTANTS_H

#include "gfx/light.h"

#include <glm/glm.hpp>

#include <string>

namespace gfx {

// Width of a shadow map in pixels.
const unsigned int SHADOW_MAP_WIDTH = 4096;
// Height of a shadow map in pixels.
const unsigned int SHADOW_MAP_HEIGHT = 4096;
// The maximum number of point lights in a scene.
const unsigned int MAX_POINT_LIGHTS = 3;
// The number of samples for MSAA.
const unsigned int MSAA_SAMPLES = 4;
// The number of samples to take for IBL. Note that this must match the number in main.frag and
// must be in base 2.
const unsigned int NUM_IBL_SAMPLES = 32;
// Path to the main vertex shader.
const std::string MAIN_VERTEX_SHADER_PATH = "shaders/main.vert";
// Path to the main fragment shader.
const std::string MAIN_FRAG_SHADER_PATH = "shaders/main.frag";
// Path to the HDR vertex shader.
const std::string HDR_VERTEX_SHADER_PATH = "shaders/hdr.vert";
// Path to the HDR fragment shader.
const std::string HDR_FRAG_SHADER_PATH = "shaders/hdr.frag";
// Path to the skybox vertex shader.
const std::string SKYBOX_VERTEX_SHADER_PATH = "shaders/skybox.vert";
// Path to the skybox fragment shader.
const std::string SKYBOX_FRAG_SHADER_PATH = "shaders/skybox.frag";
// Path to the directional light shadow mapping vertex shader.
const std::string DIR_SHADOW_VERTEX_SHADER_PATH = "shaders/directional_shadow.vert";
// Path to the directional light shadow mapping fragment shader.
const std::string DIR_SHADOW_FRAG_SHADER_PATH = "shaders/directional_shadow.frag";

}
#endif // GFX_CONSTANTS_H
