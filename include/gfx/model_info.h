#ifndef GFX_MODEL_INFO_H
#define GFX_MODEL_INFO_H

#include "gfx/color.h"
#include "gfx/mesh.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gfx {

class ModelInfo {
  public:
    std::vector<gfx::Mesh> meshes;
    gfx::Color color;

    ModelInfo(std::string model_path, gfx::Color color);
  private:
    // TODO: Pull from a global pool of managed buffers instead of having a buffer per model.
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

}
#endif // GFX_MODEL_INFO_H
