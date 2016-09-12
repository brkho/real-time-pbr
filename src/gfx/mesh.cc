#include "gfx/exceptions.h"
#include "gfx/mesh.h"
#include "gfx/util.h"

#include <iostream>

gfx::Mesh::Mesh(std::vector<gfx::Vertex>* vertices, std::vector<GLuint>* indices, bool should_map) :
    vao{0}, vbo{0}, ebo{0}, material{gfx::Material("diffuse", "specular", 75.0f)},
    vertices{vertices}, indices{indices} {
  if (should_map) {
    gfx::Mesh::Map();
  }
}

gfx::Mesh::~Mesh() {
  // if(gfx::Mesh::IsMapped()) {
  //   gfx::Mesh::Unmap();
  // }
  // delete vertices;
  // delete indices;
}

bool gfx::Mesh::IsMapped() {
  return (vao != 0 || vbo != 0 || ebo != 0);
}

void gfx::Mesh::Map() {
  if (gfx::Mesh::IsMapped()) {
    throw gfx::BuffersAlreadyMappedException();
  }
  // Generate buffers.
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  // Set up the VBO.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(),
      GL_STATIC_DRAW);

  // Set up the EBO.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(GLuint),
      &indices->front(), GL_STATIC_DRAW);

  // Specify the vertex attributes.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      (GLvoid*) offsetof(Vertex, position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      (GLvoid*) offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void gfx::Mesh::Unmap() {
  if (!gfx::Mesh::IsMapped()) {
    throw gfx::BuffersNotYetMappedException();
  }
  glDeleteBuffers(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  vao = 0;
  vbo = 0;
  ebo = 0;
}

void gfx::Mesh::Remap() {
  gfx::Mesh::Unmap();
  gfx::Mesh::Map();
}

GLuint gfx::Mesh::GetNumberOfIndices() {
  return indices->size();
}
