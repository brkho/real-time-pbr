#include "gfx/exceptions.h"
#include "gfx/model_instance.h"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

gfx::ModelInstance::ModelInstance(gfx::ModelInfo* model_info) :
    position{glm::vec3{0.0f, 0.0f, 0.0f}}, scale(glm::vec3{0.0f, 0.0f, 0.0f}),
    rotation{glm::quat{1.0f, 0.0f, 0.0f, 0.0f}}, color{gfx::Color(1.0f, 1.0f, 1.0f)},
    model_info{model_info} {
  gfx::ModelInstance::CalculateTransforms();
}

void gfx::ModelInstance::Draw(GLuint program) {
  if (!model_info->IsMapped()) {
    throw gfx::BuffersNotYetMappedException();
  }
  GLint model_location = glGetUniformLocation(program, "model");
  glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_transform));
  // Draw all meshes.
  for (auto &mesh : model_info->meshes) {
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glDrawElements(GL_TRIANGLES, mesh.GetNumberOfIndices(), GL_UNSIGNED_INT, 0);
  }
}

void gfx::ModelInstance::CalculateTransforms() {
  model_transform = glm::mat4();
  model_transform = glm::scale(model_transform, scale);
  model_transform = glm::mat4_cast(rotation) * model_transform;
  model_transform = glm::translate(model_transform, position);
}
