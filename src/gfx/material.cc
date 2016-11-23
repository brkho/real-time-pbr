#include "gfx/material.h"

#include <iostream>

gfx::Material::Material(gfx::ShaderType shader_type, GLuint albedo_handle, GLuint specular_handle,
    GLuint gloss_handle, GLuint ior_handle, GLuint normal_handle, GLuint ao_handle,
    GLfloat ambient) : ambient_coefficient{ambient}, shader_type{shader_type},
    albedo_handle{albedo_handle}, specular_handle{specular_handle}, gloss_handle{gloss_handle},
    ior_handle{ior_handle}, normal_handle{normal_handle}, ao_handle{ao_handle} {}

gfx::Material::~Material() {
  return;
}

void gfx::Material::UseMaterial(GLuint program) {
  GLint shader_type_location = glGetUniformLocation(program, "shader_type");
  glUniform1f(shader_type_location, shader_type);

  GLint ambient_location = glGetUniformLocation(program, "ambient_coefficient");
  glUniform1f(ambient_location, ambient_coefficient);

  GLint albedo_enabled_location = glGetUniformLocation(program, "albedo_enabled");
  glUniform1i(albedo_enabled_location, albedo_handle != 0);
  if (albedo_handle != 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedo_handle);
    glUniform1i(glGetUniformLocation(program, "albedo_map"), 0);
  }

  GLint specular_enabled_location = glGetUniformLocation(program, "specular_enabled");
  glUniform1i(specular_enabled_location, specular_handle != 0);
  if (specular_handle != 0) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_handle);
    glUniform1i(glGetUniformLocation(program, "specular_map"), 1);
  }

  GLint gloss_enabled_location = glGetUniformLocation(program, "gloss_enabled");
  glUniform1i(gloss_enabled_location, gloss_handle != 0);
  if (gloss_handle != 0) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gloss_handle);
    glUniform1i(glGetUniformLocation(program, "gloss_map"), 2);
  }

  GLint ior_enabled_location = glGetUniformLocation(program, "ior_enabled");
  glUniform1i(ior_enabled_location, ior_handle != 0);
  if (ior_handle != 0) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ior_handle);
    glUniform1i(glGetUniformLocation(program, "ior_map"), 3);
  }

  GLint normal_enabled_location = glGetUniformLocation(program, "normal_enabled");
  glUniform1i(normal_enabled_location, normal_handle != 0);
  if (normal_handle != 0) {
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, normal_handle);
    glUniform1i(glGetUniformLocation(program, "normal_map"), 4);
  }

  GLint ao_enabled_location = glGetUniformLocation(program, "ao_enabled");
  glUniform1i(ao_enabled_location, ao_handle != 0);
  if (ao_handle != 0) {
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, ao_handle);
    glUniform1i(glGetUniformLocation(program, "ao_map"), 5);
  }
}

void gfx::Material::RemoveTexture(GLuint id) {
  if (albedo_handle == id) {
    albedo_handle = 0;
  }
  if (specular_handle == id) {
    specular_handle = 0;
  }
}
