#include "gfx/environment.h"
#include "gfx/exceptions.h"

#include <iostream>
#include <stb_image.h>

gfx::Environment::Environment(std::string skybox_path, float skybox_blur) : environment_handle{0},
    skybox_blur{skybox_blur} {
  // Load the image.
  int width, height, num_components;
  float* image_data = stbi_loadf(skybox_path.c_str(), &width, &height, &num_components, 0);
  if (image_data == nullptr || (num_components != 3 && num_components != 4)) {
    throw gfx::CannotLoadTextureException();
  }
  GLenum image_format = num_components == 4 ? GL_RGBA : GL_RGB;

  // Transfer the texture to OpenGL.
  glGenTextures(1, &environment_handle);
  glBindTexture(GL_TEXTURE_2D, environment_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, image_format, GL_FLOAT, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(image_data);
  glBindTexture(GL_TEXTURE_2D, 0);
}
