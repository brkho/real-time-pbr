#include "gfx/camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

gfx::Camera::Camera() : camera_position{glm::vec3{0.0f, 0.0f, 3.0f}},
    camera_target{glm::vec3{0.0f, 0.0f, 0.0f}}, camera_up{glm::vec3{0.0f, 1.0f, 0.0f}} {
  gfx::Camera::UpdateViewTransform();
}

gfx::Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up) :
    camera_position{position}, camera_target{target}, camera_up{up} {
  gfx::Camera::UpdateViewTransform();
}

glm::vec3 gfx::Camera::GetUpVector() {
  return glm::row(view_transform, 1);
}

glm::vec3 gfx::Camera::GetRightVector() {
  return glm::row(view_transform, 0);
}

glm::vec3 gfx::Camera::GetForwardVector() {
  return glm::row(view_transform, 2);
}

void gfx::Camera::UpdateViewTransform() {
  view_transform = glm::lookAt(camera_position, camera_target, camera_up);
}

glm::mat4 gfx::Camera::GetViewTransform() {
  gfx::Camera::UpdateViewTransform();
  return view_transform;
}
