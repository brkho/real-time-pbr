#include "gfx/exceptions.h"
#include "gfx/model_instance.h"
#include "gfx/util.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

gfx::ModelInstance::ModelInstance(gfx::ModelInfo* model_info) :
    position{glm::vec3{0.0f, 0.0f, 0.0f}}, scale(glm::vec3{1.0f, 1.0f, 1.0f}),
    rotation{glm::quat{1.0f, 0.0f, 0.0f, 0.0f}}, model_info{model_info} {
  gfx::ModelInstance::Update();
}

void gfx::ModelInstance::Draw(GLuint program) {
  if (!model_info->IsMapped()) {
    throw gfx::BuffersNotYetMappedException();
  }
  GLint model_location = glGetUniformLocation(program, "model_transform");
  glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_transform));
  GLint normal_location = glGetUniformLocation(program, "normal_transform");
  glUniformMatrix4fv(normal_location, 1, GL_FALSE, glm::value_ptr(normal_transform));
  // Draw all meshes.
  for (auto &mesh : model_info->meshes) {
    glBindVertexArray(mesh.vao);
    mesh.material.UseMaterial(program);
    glDrawElements(GL_TRIANGLES, mesh.GetNumberOfIndices(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

void gfx::ModelInstance::Update() {
  model_transform = glm::mat4();
  model_transform = glm::scale(model_transform, scale);
  model_transform = glm::mat4_cast(rotation) * model_transform;
  model_transform = glm::translate(model_transform, position);
  normal_transform = glm::transpose(glm::inverse(model_transform));
}
