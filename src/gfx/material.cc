#include "gfx/material.h"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

gfx::Material::Material(ShaderType shader_type, MapInfo albedo_info, MapInfo metallic_info,
    MapInfo roughness_info, MapInfo normal_info, MapInfo ao_info, GLfloat ambient) :
    ambient_coefficient{ambient}, albedo_info{albedo_info}, metallic_info{metallic_info},
    roughness_info{roughness_info}, normal_info{normal_info}, ao_info{ao_info},
    shader_type{shader_type} {}

gfx::Material::~Material() {
  return;
}

void gfx::Material::UseMaterial(GLuint program) {
  GLint shader_type_location = glGetUniformLocation(program, "shader_type");
  glUniform1f(shader_type_location, shader_type);

  GLint ambient_location = glGetUniformLocation(program, "ambient_coefficient");
  glUniform1f(ambient_location, ambient_coefficient);

  BindMap(program, albedo_info, "albedo", 1);
  BindMap(program, metallic_info, "metallic", 2);
  BindMap(program, roughness_info, "roughness", 3);
  BindMap(program, normal_info, "normal", 4);
  BindMap(program, ao_info, "ao", 5);
}

void gfx::Material::RemoveTexture(GLuint id) {
  if (albedo_info.handle == id) {
    albedo_info.handle = 0;
  } else if (metallic_info.handle == id) {
    metallic_info.handle = 0;
  } else if (roughness_info.handle == id) {
    roughness_info.handle = 0;
  } else if (normal_info.handle == id) {
    normal_info.handle = 0;
  } else if (ao_info.handle == id) {
    ao_info.handle = 0;
  }
}

void gfx::Material::BindMap(GLuint program, MapInfo info, std::string map_type,
    GLuint texture_unit) {
  glUniform1i(glGetUniformLocation(program, (map_type + "_map.enabled").c_str()), info.handle != 0);
  if (info.handle == 0) {
    glUniform3fv(glGetUniformLocation(program, (map_type + "_map.default_value").c_str()), 1,
        glm::value_ptr(info.value));
  } else {
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, info.handle);
    glUniform1i(glGetUniformLocation(program, (map_type + "_map.map").c_str()), texture_unit);
  }
}
