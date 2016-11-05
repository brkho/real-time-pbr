// This class defines a game window backed by GLFW that abstracts away much of the OpenGL
// operations. In addition, it will also manage the camera, shaders, and program(s). The GameWindow
// will hold references to the Camera and Lights and will require an input of ModelInstances to
// draw. The full feature set of the GLFW can be accessed via the window property.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_GAME_WINDOW_H
#define GFX_GAME_WINDOW_H

#include "gfx/camera.h"
#include "gfx/color.h"
#include "gfx/constants.h"
#include "gfx/directional_light.h"
#include "gfx/mesh.h"
#include "gfx/model_instance.h"
#include "gfx/point_light.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace gfx {

class GameWindow {
  public:
    // A reference to the camera used to render the scene.
    gfx::Camera* camera;

    // A reference to the underlying GLFW window.
    GLFWwindow* window;

    // Constructor with a width, height, path to the vertex shader, path to the fragment shader,
    // referenceto the camera, field of view, and the buffer clear color.
    GameWindow(int width, int height, std::string main_vertex_path, std::string main_fragment_path,
        std::string hdr_vertex_path, std::string hdr_fragment_path, gfx::Camera* camera, float fov,
        gfx::Color color);

    // Constructor with a width, height, path to the vertex shader, path to the fragment shader,
    // referenceto the camera, field of view, and the buffer clear color. This defaults the FOV to
    // 45 degrees and the buffer clear color to black.
    GameWindow(int width, int height, std::string main_vertex_path, std::string main_fragment_path,
        std::string hdr_vertex_path, std::string hdr_fragment_path, gfx::Camera* camera);

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

    // Updates the directional light uniform.
    void UpdateDirectionalLight();

    // Adds a point light to the scene.
    void AddPointLight(gfx::PointLight* point_light);

    // Removes a point light from the scene.
    void RemovePointLight(gfx::PointLight* point_light);

    // Updates a point light. This must be called after any changes to the point light's fields for
    // it to be reflected for rendering in the engine.
    void UpdatePointLight(gfx::PointLight* point_light);

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

    // Compeletes the rendering started by PrepareRender. This takes the accumulated render on the
    // HDR buffer and tone maps it onto the display buffer. It then swaps the buffer so the
    // rendered image can actually be seen.
    void FinishRender();

  private:
    // The field of view for the window.
    GLfloat field_of_view;

    // The main shader program used by the GameWindow. This outputs to a HDR framebuffer which is
    // in turn rendered with the hdr_program.
    GLuint program;

    // The HDR shader program that takes the output of the main shader program and tone maps it
    // into LDR space for the final render.
    GLuint hdr_program;

    // The Framebuffer Object (with greater floating point precision) that is written to by the
    // main shader and then subsequently used for rendering by the HDR program.
    GLuint hdr_fbo;

    // The color buffer that s the render target of the main shader.
    GLuint hdr_color_buffer;

    // The quad used by the HDR shader for drawing the HDR texture output by the main shader.
    gfx::Mesh* draw_quad;

    // The vertices that make up the draw_quad.
    std::vector<gfx::Vertex>* quad_vertices;

    // The elements that make up the draw_quad.
    std::vector<GLuint>* quad_elements;

    // The matrix used for the perspective projection.
    glm::mat4 perspective_projection;

    // The directional light of the scene. This will be nullptr if there is no directional light.
    gfx::DirectionalLight* directional_light;

    // The array of pointers to point lights.
    gfx::PointLight* point_lights[gfx::MAX_POINT_LIGHTS];

    // An associative array mapping pointers to point lights back to an index into point_lights.
    std::unordered_map<gfx::PointLight*, unsigned int> point_lights_reverse;

    // Given a path to the shader and a shader type, compile the shader.
    GLuint CompileShader(std::string path, GLenum shader_type);

    // Given a path to the vertex shader and the fragment shader, compile the shaders and link them
    // into a shader program.
    GLuint LinkProgram(std::string vertex_path, std::string fragment_path);

    // Initializes the game window.
    void InitializeGameWindow(int width, int height, gfx::Color color);

    // Updates the perspective projection with the width, height, and field of view.
    void UpdatePerspectiveProjection(int width, int height);

    // Helper function to find the index of a point light using the reverse map. This throws if the
    // point light is not yet added.
    unsigned int GetAndValidatePointLightIndex(gfx::PointLight* point_light);

    // Helper function to turn a vec3 position into a Vertex to be consumed by the Mesh class with
    // non-important values for normal, tangent, and UV.
    gfx::Vertex PositionToQuadVertex(glm::vec3 position);
};

}
#endif // GFX_GAME_WINDOW_H
