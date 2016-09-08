// This class provides a representation of a mesh. A mesh stores a set of vertices, indicies on
// those verticies forming the model, the VAO, the VBO, the EBO, and the material. The class
// implements the Mappable interface which maps the mesh data to OpenGL managed buffers.

// Brian Ho (brian@dropbox.com)

#ifndef GFX_MESH_H
#define GFX_MESH_H

#include "gfx/mappable.h"
#include "gfx/material.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

namespace gfx {

// Abstraction that describes a vertex.
struct Vertex {
  // Location of the vertex.
  glm::vec3 position;
  // Normal of the vertex.
  glm::vec3 normal;
};

class Mesh : public gfx::Mappable {
  public:
    // TODO: Pull from a global pool of managed buffers instead of having a buffer per model.
    // Stores the integer handle to an OpenGL managed VAO and is 0 if unmapped.
    GLuint vao;
    // Stores the integer handle to an OpenGL managed VBO and is 0 if unmapped.
    GLuint vbo;
    // Stores the integer handle to an OpenGL managed EBO and is 0 if unmapped.
    GLuint ebo;
    // The material of the mesh.
    gfx::Material material;

    // Create a Mesh with a list of verticies and indices. The should_map param specifies whether
    // Map() should be called immediately by the constructor.
    Mesh(std::vector<Vertex>* vertices, std::vector<GLuint>* indices, bool should_map);

    // Destroys the Mesh by unmapping and freeing the vertex and index vectors.
    ~Mesh();

    // Returns whether or not the Mesh is currently mapped with any buffers.
    bool IsMapped();

    // If the model is unmapped, set up the VAO, EBO, and EBO by mapping the vertex data to the
    // buffers.
    void Map();

    // If the model is mapped, delete the VAO, EBO, and EBO.
    void Unmap();
    
    // If the model is mapped, unmap everything and then map it again.
    void Remap();

    // Gets the number of indices for drawing purposes.
    GLuint GetNumberOfIndices();
  private:
    // List of vertices.
    std::vector<Vertex>* vertices;
    // List of indices.
    std::vector<GLuint>* indices;
};

}
#endif // GFX_MESH_H
