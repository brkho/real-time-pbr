#include "gfx/mesh.h"

#include <iostream>

gfx::Mesh::Mesh(std::vector<gfx::Vertex> vertices, std::vector<GLuint> indices) :
    vertices{vertices}, indices{indices} {
  vao = 0;
  vbo = 0;
  ebo = 0;
}
