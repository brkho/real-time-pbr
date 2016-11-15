#define STB_IMAGE_IMPLEMENTATION

#include "gfx/exceptions.h"
#include "gfx/texture_manager.h"

#include <stb_image.h>

#include <iostream>

gfx::TextureManager::TextureManager() : path_to_id_map() {}

GLuint gfx::TextureManager::GetTextureHandle(std::string path, bool convert_to_linear) {
  // If we have already loaded this texture, simply return the cached ID.
  auto path_it = path_to_id_map.find(path);
  if (path_it != path_to_id_map.end()) {
    return path_it->second;
  }

  // Load the image.
  int width, height, bits_per_pixel;
  unsigned char* image_data = stbi_load(path.c_str(), &width, &height, &bits_per_pixel, 0);
  if (image_data == nullptr || (bits_per_pixel != 3 && bits_per_pixel != 4)) {
    throw gfx::CannotLoadTextureException();
  }
  GLenum image_format = bits_per_pixel == 4 ? GL_RGBA : GL_RGB;

  // Transfer the texture to OpenGL.
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  GLenum engine_format = convert_to_linear ? GL_SRGB : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, engine_format, width, height, 0, image_format, GL_UNSIGNED_BYTE,
      image_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image_data);
  glBindTexture(GL_TEXTURE_2D, 0);
  path_to_id_map[path] = texture;
  return texture;
}

void gfx::TextureManager::FreeTexture(GLuint id) {
  for (auto it = path_to_id_map.begin(); it != path_to_id_map.end(); ++it) {
    if (it->second == id) {
      path_to_id_map.erase(it);
      break;
    }
  }
  glDeleteTextures(1, &id);
}
