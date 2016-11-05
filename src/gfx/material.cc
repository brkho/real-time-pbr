#include "gfx/material.h"

#include <iostream>

gfx::Material::Material(GLuint diffuse_handle, GLuint specular_handle, GLuint normal_handle,
    GLfloat shininess, GLfloat ambient) : shininess{shininess}, ambient_coefficient{ambient},
    diffuse_handle{diffuse_handle}, specular_handle{specular_handle},
    normal_handle{normal_handle} {}

gfx::Material::Material(GLuint diffuse_handle, GLuint specular_handle, GLuint normal_handle,
    GLfloat shininess) : Material(diffuse_handle, specular_handle, normal_handle, shininess,
    0.001f) {}

gfx::Material::~Material() {
  return;
}

void gfx::Material::UseMaterial(GLuint program) {
  GLint ambient_location = glGetUniformLocation(program, "ambient_coefficient");
  glUniform1f(ambient_location, ambient_coefficient);
  GLint shininess_location = glGetUniformLocation(program, "shininess");
  glUniform1f(shininess_location, shininess);

  GLint diffuse_enabled_location = glGetUniformLocation(program, "diffuse_enabled");
  glUniform1i(diffuse_enabled_location, diffuse_handle != 0);
  if (diffuse_handle != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_handle);
    glUniform1i(glGetUniformLocation(program, "diffuse_texture"), 0);
  }

  GLint specular_enabled_location = glGetUniformLocation(program, "specular_enabled");
  glUniform1i(specular_enabled_location, specular_handle != 0);
  if (specular_handle != 0) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_handle);
    glUniform1i(glGetUniformLocation(program, "specular_texture"), 1);
  }

  GLint normal_enabled_location = glGetUniformLocation(program, "normal_enabled");
  glUniform1i(normal_enabled_location, normal_handle != 0);
  if (normal_handle != 0) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normal_handle);
    glUniform1i(glGetUniformLocation(program, "normal_map"), 2);
  }
}

void gfx::Material::RemoveTexture(GLuint id) {
  if (diffuse_handle == id) {
    diffuse_handle = 0;
  }
  if (specular_handle == id) {
    specular_handle = 0;
  }
}
