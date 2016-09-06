#ifndef GFX_MESH_H
#define GFX_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

namespace gfx {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

class Mesh {
  public:
    // TODO: Pull from a global pool of managed buffers instead of having a buffer per model.
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
  private:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

}
#endif // GFX_MESH_H
