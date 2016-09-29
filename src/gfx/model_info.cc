#include "gfx/exceptions.h"
#include "gfx/model_info.h"

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>
#include <memory>

gfx::ModelInfo::ModelInfo(std::string model_path, gfx::TextureManager* manager, bool should_map) :
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
    const aiMaterial* assimp_material = scene->mMaterials[assimp_mesh->mMaterialIndex];
    float shininess;
    assimp_material->Get(AI_MATKEY_SHININESS, shininess);
    GLuint diffuse_handle = LoadTexture(assimp_material, aiTextureType_DIFFUSE, manager);
    GLuint specular_handle = LoadTexture(assimp_material, aiTextureType_SPECULAR, manager);
    std::shared_ptr<gfx::Material> material(new gfx::Material(
        diffuse_handle, specular_handle, shininess));

    std::vector<gfx::Vertex>* vertices = new std::vector<gfx::Vertex>();
    for (unsigned int j = 0; j < assimp_mesh->mNumVertices; ++j) {
      glm::vec3 position{-assimp_mesh->mVertices[j].x, assimp_mesh->mVertices[j].z,
          assimp_mesh->mVertices[j].y};
      glm::vec3 normal{-assimp_mesh->mNormals[j].x, assimp_mesh->mNormals[j].z,
          assimp_mesh->mNormals[j].y};
      glm::vec2 uv{assimp_mesh->mTextureCoords[0][j].x, assimp_mesh->mTextureCoords[0][j].y};

      gfx::Vertex vertex{position, normal, uv};
      vertices->push_back(vertex);
    }

    std::vector<GLuint>* indices = new std::vector<GLuint>();
    for (unsigned int j = 0; j < assimp_mesh->mNumFaces; ++j) {
      aiFace assimp_face = assimp_mesh->mFaces[j];
      for (unsigned int k = 0; k < assimp_face.mNumIndices; k++) {
        indices->push_back(assimp_face.mIndices[k]);
      }
    }
    meshes.push_back(gfx::Mesh(vertices, indices, material, should_map));
  }
}

GLuint gfx::ModelInfo::LoadTexture(const aiMaterial* material, aiTextureType type,
    gfx::TextureManager* manager) {
  aiString path_struct;
  material->GetTexture(type, 0, &path_struct, nullptr, nullptr, nullptr, nullptr, nullptr);
  std::string path(path_struct.data);
  return path.length() == 0 ? 0 : manager->GetTextureHandle(assets_path + path);
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
