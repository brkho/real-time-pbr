// This class provides a representation for an environment. This class holds the image data for the
// environment (in the form of an HDR image). Environments can be used for either rendering
// skyboxes or as a source of ambient lighting.

// Brian Ho (brian@dropbox.com)

#ifndef GFX_ENVIRONMENT_H
#define GFX_ENVIRONMENT_H

#include <glad/glad.h>

#include <string>

namespace gfx {

// TODO(brkho): Implement more types of supported textures other than parabolic.
class Environment {
  public:
    // Handle to the OpenGL managed texture for the environment. This is an HDR texture.
    GLuint environment_handle;
    // How much to blur the skybox (essentially what mipmap level to sample from).
    GLfloat skybox_blur;

    // Constructor specifying a path to the HDR skybox image and a skybox_blur value. Unlike normal
    // texture uploades that go through TextureManager, this does not prevent duplicates.
    Environment(std::string skybox_path, GLfloat skybox_blur);

    // Constructor for an environment without a skybox blur.
    Environment(std::string skybox_path) : Environment(skybox_path, 0.0f) {}
};

}
#endif // GFX_ENVIRONMENT_H
