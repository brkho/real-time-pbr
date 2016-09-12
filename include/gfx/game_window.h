// This class defines a game window backed by GLFW that abstracts away much of the OpenGL
// operations. In addition, it will also manage the camera, shaders, and program. The GameWindow
// will hold references to the Camera and Lights and will require an input of ModelInstances to
// draw. The full feature set of the GLFW can be accessed via the window property.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_GAME_WINDOW_H
#define GFX_GAME_WINDOW_H

#include "gfx/camera.h"
#include "gfx/color.h"
#include "gfx/directional_light.h"
#include "gfx/model_instance.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

namespace gfx {

class GameWindow {
  public:
    // A reference to the camera used to render the scene.
    gfx::Camera* camera;

    // A reference to the underlying GLFW window.
    GLFWwindow* window;

    // Constructor with a width, height, path to the vertex shader, path to the fragment shader,
    // referenceto the camera, field of view, and the buffer clear color.
    GameWindow(int width, int height, std::string vertex_path, std::string fragment_path,
        gfx::Camera* camera, float fov, gfx::Color color);

    // Constructor with a width, height, path to the vertex shader, path to the fragment shader,
    // referenceto the camera, field of view, and the buffer clear color. This defaults the FOV to
    // 45 degrees and the buffer clear color to black.
    GameWindow(int width, int height, std::string vertex_path, std::string fragment_path,
        gfx::Camera* camera);

    // TODO: Actually clean up after ourselves with the Rule of Three.

    // Returns whether the window is currently running (used to determine when to quit the main
    // game loop).
    bool IsRunning();

    // Updates the dimensions of the window and recalculates the perspective projection.
    void UpdateDimensions(int width, int height);

    // Updates the field of view and recalculates the perspective projection.
    void UpdateFieldOfView(float fov);

    // Sets the buffer clear color.
    void SetBufferClearColor(gfx::Color color);

    // Set the directional light, replacing whatever the current directional light is.
    void SetDirectionalLight(gfx::DirectionalLight* directional_light);

    // Remove the directional light.
    void UnsetDirectionalLight();

    // Polls the GLFW window for events and invokes the proper callbacks.
    void PollForEvents();

    // Gets the time in seconds since the window was created.
    double GetElapsedTime();

    // This must be called every frame before drawing any ModelInstances to the screen. This sets
    // the state of the OpenGL context so that we can begin drawing the next frame. After this is
    // called, the caller shouldn't change the game state and should only call RenderModel until
    // the render is completed with FinishRender.
    void PrepareRender();

    // Draws a given ModelInstance. Note that this must be called in between a PrepareRender and a
    // a FinishRender.
    void RenderModel(gfx::ModelInstance* model_instance);

    // Complets the rendering started by PrepareRender. This swaps the buffer so the rendered image
    // can actually be seen.
    void FinishRender();

  private:
    // The field of view for the window.
    GLfloat field_of_view;

    // The shader program used by the GameWindow.
    GLuint program;

    // The matrix used for the perspective projection.
    glm::mat4 perspective_projection;

    // The directional light of the scene. This will be nullptr if there is no directional light.
    gfx::DirectionalLight* directional_light;

    // Given a path to the shader and a shader type, compile the shader.
    GLuint CompileShader(std::string path, GLenum shader_type);

    // Given a path to the vertex shader and the fragment shader, compile the shaders and link them
    // into a shader program.
    GLuint LinkProgram(std::string vertex_path, std::string fragment_path);

    // Initializes the game window.
    void InitializeGameWindow(int width, int height, gfx::Color color);

    // Updates the perspective projection with the width, height, and field of view.
    void UpdatePerspectiveProjection(int width, int height);
};

}
#endif // GFX_GAME_WINDOW_H
