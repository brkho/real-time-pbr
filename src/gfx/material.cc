#include "gfx/material.h"
#include <iostream>

gfx::Material::Material(std::string diffuse_name, std::string specular_name, GLfloat shininess,
    GLfloat ambient) : shininess{shininess}, ambient_coefficient{ambient} {
  diffuse_handle = 0;
  specular_handle = 0;
}

gfx::Material::Material(std::string diffuse_name, std::string specular_name, GLfloat shininess) :
    Material(diffuse_name, specular_name, shininess, 0.03f) {}

gfx::Material::~Material() {
  return;
}

void gfx::Material::UseMaterial(GLuint program) {
  GLint ambient_location = glGetUniformLocation(program, "ambient_coefficient");
  glUniform1f(ambient_location, ambient_coefficient);
  GLint shininess_location = glGetUniformLocation(program, "shininess");
  glUniform1f(shininess_location, shininess);
}

void gfx::Material::RemoveTexture(GLuint id) {
  if (diffuse_handle == id) {
    diffuse_handle = 0;
  }
  if (specular_handle == id) {
    specular_handle = 0;
  }
}
