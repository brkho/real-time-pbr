#include "gfx/material.h"
#include <iostream>

gfx::Material::Material(std::string diffuse_name, std::string specular_name, gfx::Color color,
    GLfloat shininess, GLfloat ambient) : color{color}, shininess{shininess},
    ambient_coefficient{ambient} {
  diffuse_handle = 0;
  specular_handle = 0;
}

gfx::Material::Material(std::string diffuse_name, std::string specular_name, GLfloat shininess) :
    Material(diffuse_name, specular_name, gfx::Color(1.0f, 1.0f, 1.0f), shininess,
    0.15f) {}

gfx::Material::~Material() {
  return;
}

void gfx::Material::UseMaterial(GLuint program) {
  GLint ambient_location = glGetUniformLocation(program, "ambient_coefficient");
  glUniform1f(ambient_location, ambient_coefficient);
  GLint color_location = glGetUniformLocation(program, "base_color");
  glUniform4f(color_location, color.r, color.g, color.b, color.a);
}
