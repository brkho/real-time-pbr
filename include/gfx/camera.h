// This class defines a camera to be used with the GameWindow class for rendering a scene. A camera
// contains all information needed to produce the view transform: position, target, and up vector.
// In additon, it also has functions for getting the up, right, and forward vectors of the
// resulting transform.

// Brian Ho (brian@brkho.com)

#ifndef GFX_CAMERA_H
#define GFX_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gfx {

class Camera {
  public:
    // The position vector for calculating the view transform.
    glm::vec3 camera_position;

    // The target vector for calculating the view transform.
    glm::vec3 camera_target;

    // The up vector for calculating the view transform.
    glm::vec3 camera_up;

    // Constructs a camera defaulting to position (0.0f, 0.0f, 3.0f), target (0.0f, 0.0f, 0.0f),
    // and up (0.0f, 0.0f, 1.0f).
    Camera();

    // Constructs a camera with supplied values.
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

    // Gets the actual up vector of the view transform.
    glm::vec3 GetUpVector();

    // Gets the actual right vector of the view transform.
    glm::vec3 GetRightVector();

    // Gets the actual forward vector of the view transform.
    glm::vec3 GetForwardVector();

    // Gets a view transform by first calling UpdateViewTransform().
    glm::mat4 GetViewTransform();

  private:
    // The view transform created by a calling lookAt with the camera position, target, and up.
    glm::mat4 view_transform;

    // Updates the view transform based on the current state of the Camera.
    void UpdateViewTransform();
};

}
#endif // GFX_CAMERA_H
