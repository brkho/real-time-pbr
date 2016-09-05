#ifndef GFX_MODEL_INFO_H
#define GFX_MODEL_INFO_H

#include "gfx/color.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

namespace gfx {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

class ModelInfo {
  public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    ModelInfo(std::vector<Vertex> vertices, std::vector<GLuint> indices);
    void Draw();
  private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

}
#endif // GFX_MODEL_INFO_H