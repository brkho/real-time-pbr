#include "gfx/material.h"

gfx::Material::Material(std::string diffuse_name, std::string specular_name, GLfloat shininess) :
    shininess{shininess} {
  diffuse_handle = 0;
  specular_handle = 0;
}

void gfx::Material::UseMaterial() {
  return;
}
