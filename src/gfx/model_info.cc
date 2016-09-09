#include "gfx/exceptions.h"
#include "gfx/model_info.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <iostream>

gfx::ModelInfo::ModelInfo(std::string model_path, bool should_map) :
    meshes{std::vector<gfx::Mesh>()} {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(model_path,
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
    throw gfx::AssimpInvalidFileException();
  }

  for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
    const aiMesh* assimp_mesh = scene->mMeshes[i];
    std::vector<gfx::Vertex>* vertices = new std::vector<gfx::Vertex>();
    for (unsigned int j = 0; j < assimp_mesh->mNumVertices; ++j) {
      glm::vec3 position{assimp_mesh->mVertices[j].x, assimp_mesh->mVertices[j].y,
          assimp_mesh->mVertices[j].z};
      glm::vec3 normal{assimp_mesh->mNormals[j].x, assimp_mesh->mNormals[j].y,
          assimp_mesh->mNormals[j].z};
      gfx::Vertex vertex{position};
      vertices->push_back(vertex);
    }

    std::vector<GLuint>* indices = new std::vector<GLuint>();
    for (unsigned int j = 0; j < assimp_mesh->mNumFaces; ++j) {
      aiFace assimp_face = assimp_mesh->mFaces[j];
      for (unsigned int k = 0; k < assimp_face.mNumIndices; k++) {
        indices->push_back(assimp_face.mIndices[k]);
      }
    }
    meshes.push_back(gfx::Mesh(vertices, indices, should_map));
  }
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
