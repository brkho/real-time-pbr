#include "gfx/exceptions.h"
#include "gfx/model_info.h"

#include <glm/glm.hpp>

#include <cmath>
#include <iostream>
#include <memory>

gfx::ModelInfo::ModelInfo(std::string model_path, gfx::TextureManager* manager, bool should_map) :
    meshes{std::vector<gfx::Mesh>()} {

  std::ifstream input_file {model_path};
  if (!input_file) {
    throw gfx::CannotOpenEOFileException();
  }

  // Read the shader type.
  char shader_type_value;
  input_file.read(&shader_type_value, 1);
  if ((size_t)shader_type_value >= gfx::shader_map.size()) {
    throw gfx::InvalidShaderTypeException();
  }
  gfx::ShaderType shader_type = gfx::shader_map[(size_t)shader_type_value];

  // Get the material info.
  GLuint albedo_handle = LoadMap(&input_file, manager, true);
  GLuint specular_handle = LoadMap(&input_file, manager, false);
  GLuint gloss_handle = LoadMap(&input_file, manager, false);
  GLuint ior_handle = LoadMap(&input_file, manager, false);
  GLuint normal_handle = LoadMap(&input_file, manager, false);
  GLuint ao_handle = LoadMap(&input_file, manager, false);
  std::shared_ptr<gfx::Material> material(new gfx::Material(shader_type, albedo_handle,
      specular_handle, gloss_handle, ior_handle, normal_handle, ao_handle, 0.05));

  // Copy the vertices directly into memory.
  size_t num_vertices;
  input_file.read((char*)(&num_vertices), sizeof(size_t));
  std::vector<gfx::Vertex>* vertices = new std::vector<gfx::Vertex> {num_vertices};
  input_file.read((char*)(vertices->data()), sizeof(gfx::Vertex) * num_vertices);

  // Copy the indices directly into memory. We only need to do error checking on this last data
  // block because if the file is too long, we will know after extracting. If the file is too short
  // this will always fail even if we exhausted all data in an earlier read.
  size_t num_indices;
  input_file.read((char*)(&num_indices), sizeof(size_t));
  // Have to use resize here instead of the constructor like I do in vertices because of implicit
  // conversion from size_t to GLuint causing the compiler to think I'm actually just trying to
  // allocate a vector of size 1.
  std::vector<GLuint>* indices = new std::vector<GLuint>();
  indices->resize(num_indices);
  input_file.read((char*)(indices->data()), sizeof(GLuint) * num_indices);
  // Error checking.
  if ((size_t)input_file.gcount() != sizeof(GLuint) * num_indices || input_file.get() != EOF) {
    throw gfx::InvalidEOFileFormatException();
  }

  // TODO(brkho): One day support multiple meshes in one model.
  meshes.push_back(gfx::Mesh(vertices, indices, material, should_map));
  input_file.close();
}

gfx::ModelInfo::~ModelInfo() {
  return;
}

bool gfx::ModelInfo::IsMapped() {
  for (auto &mesh : meshes) {
    if (!mesh.IsMapped()) {
      return false;
    }
  }
  return true;
}

void gfx::ModelInfo::Map() {
  for (auto &mesh : meshes) {
    mesh.Map();
  }
}

void gfx::ModelInfo::Unmap() {
  for (auto &mesh : meshes) {
    mesh.Unmap();
  }
}

void gfx::ModelInfo::Remap() {
  gfx::ModelInfo::Unmap();
  gfx::ModelInfo::Map();
}

GLuint gfx::ModelInfo::LoadMap(std::ifstream* input_file, gfx::TextureManager* manager,
    bool convert_to_linear) {
  char num_chars;
  input_file->read(&num_chars, 1);
  if (num_chars == 0) {
    return 0;
  }

  char path[256];
  input_file->read(path, num_chars);
  return manager->GetTextureHandle(std::string {path}, convert_to_linear);
}

