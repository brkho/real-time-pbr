#define STB_IMAGE_IMPLEMENTATION

#include "gfx/exceptions.h"
#include "gfx/texture_manager.h"

#include <stb_image.h>

#include <iostream>

gfx::TextureManager::TextureManager() : path_to_id_map(), id_to_materials_map() {}

GLuint gfx::TextureManager::GetTextureHandle(std::string path, gfx::Material* material) {
  // If we have already loaded this texture, simply return the cached ID.
  auto path_it = path_to_id_map.find(path);
  if (path_it != path_to_id_map.end()) {
    id_to_materials_map[path_it->second].insert(material);
    return path_it->second;
  }

  // Load the image.
  int width, height, bits_per_pixel;
  unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &bits_per_pixel, 0);
  if (width == 0 || height == 0) {
    throw gfx::CannotLoadTextureException();
  }

  // Transfer the texture to OpenGL.
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image_data);
  glBindTexture(GL_TEXTURE_2D, 0);
  id_to_materials_map[texture].insert(material);
  path_to_id_map[path] = texture;
  return texture;
}

void gfx::TextureManager::FreeTexture(GLuint id) {
  auto id_it = id_to_materials_map.find(id);
  if (id_it != id_to_materials_map.end()) {
    auto materials = id_it->second;
    for (auto material = materials.begin(); material != materials.end(); material++) {
      (*material)->RemoveTexture(id);
    }
    id_to_materials_map.erase(id);
  }
}
