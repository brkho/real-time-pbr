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
#include "gfx/environment.h"
#include "gfx/mesh.h"
#include "gfx/model_instance.h"
#include "gfx/point_light.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace gfx {

// Bayer Matrix for ordered dithing used to combat banding in low light scenes.
// From: http://www.anisopteragames.com/how-to-fix-color-banding-with-dithering/
const char bayer_matrix[] = {
  10, 32, 8, 40, 2, 34, 10, 42,
  48, 16, 56, 24, 50, 18, 58, 26,
  12, 44, 4, 36, 14, 46, 6, 38,
  60, 28, 52, 20, 62, 30, 54, 22,
  3, 35, 11, 43, 1, 33, 9, 41,
  51, 19, 59, 27, 49, 17, 57, 25,
  15, 47, 7, 39, 13, 45, 5, 37,
  63, 31, 55, 23, 61, 29, 53, 21};
// Number of vertices in skybox_vertices.
const int num_skybox_vertices = 36;
// Vertex positions of a skybox centered at the origin and extending -1 units in each direction.
const GLfloat skybox_vertices[] = {
  -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
  -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
  1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f,
  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

class GameWindow {
  public:
    // A reference to the camera used to render the scene.
    gfx::Camera* camera;

    // A reference to the underlying GLFW window.
    GLFWwindow* window;

    // The field of view for the window.
    GLfloat field_of_view;

    // Constructor with a width, height, paths to the shaders, the reference to the camera, field
    // of view, and the buffer clear color.
    GameWindow(int width, int height, std::string main_vertex_path, std::string main_fragment_path,
        std::string hdr_vertex_path, std::string hdr_fragment_path, std::string skybox_vertex_path,
        std::string skybox_fragment_path, gfx::Camera* camera, float fov, gfx::Color color);

    // Constructor with a width, height, paths to the shaders, the reference to the camera, field
    // of view, and the buffer clear color. This defaults the FOV to 45 degrees and the buffer
    // clear color to black.
    GameWindow(int width, int height, std::string main_vertex_path, std::string main_fragment_path,
        std::string hdr_vertex_path, std::string hdr_fragment_path, std::string skybox_vertex_path,
        std::string skybox_fragment_path, gfx::Camera* camera);

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
    // the render is completed with FinishRender. This is passed an environment which is usde to
    // render the skybox. A nullptr means that no skybox is rendered.
    void PrepareRender(gfx::Environment* environment);

    // Prepares the render without a skybox.
    void PrepareRender() { PrepareRender(nullptr); }

    // Draws a given ModelInstance. Note that this must be called in between a PrepareRender and a
    // a FinishRender. An environment is passed for use in ambient lighting. A nullptr for the
    // environment means that no environment is used in ambient lighting.
    void RenderModel(gfx::ModelInstance* model_instance, gfx::Environment* environment);

    // Renders a model without an environment.
    void RenderModel(gfx::ModelInstance* model_instance) { RenderModel(model_instance, nullptr); }

    // Compeletes the rendering started by PrepareRender. This takes the accumulated render on the
    // HDR buffer and tone maps it onto the display buffer. It then swaps the buffer so the
    // rendered image can actually be seen.
    void FinishRender();

  private:
    // The main shader program used by the GameWindow. This outputs to a HDR framebuffer which is
    // in turn rendered with the hdr_program.
    GLuint program;

    // The width of the viewport.
    GLuint vp_width;

    // The height of the viewport.
    GLuint vp_height;

    // The HDR shader program that takes the output of the main shader program and tone maps it
    // into LDR space for the final render.
    GLuint hdr_program;

    // The skybox shader program that renders the skybox given panoramic HDR texture.
    GLuint skybox_program;

    // The Framebuffer Object (with greater floating point precision) that is written to by the
    // main shader and then subsequently used for rendering by the HDR program. This is
    // multisampled for MSAA.
    GLuint multisampled_hdr_fbo;

    // Color buffer used with the multisampled FBO.
    GLuint multisampled_hdr_color_buffer;

    // The handle to the texture storing the Bayer matrix used for dithering.
    GLuint matrix_handle;

    // The quad used by the HDR shader for drawing the HDR texture output by the main shader.
    gfx::Mesh* draw_quad;

    // The vertices that make up the draw_quad.
    std::vector<gfx::Vertex>* quad_vertices;

    // The elements that make up the draw_quad.
    std::vector<GLuint>* quad_elements;

    // The actual skybox mesh used by the skybox shader for rendering.
    gfx::Mesh* skybox_mesh;

    // The vertices that make up the skybox_mesh.
    std::vector<gfx::Vertex>* skybox_vertices;

    // The elements that make up the skybox_mesh.
    std::vector<GLuint>* skybox_elements;

    // The matrix used for the perspective projection.
    glm::mat4 perspective_projection;

    // The directional light of the scene. This will be nullptr if there is no directional light.
    gfx::DirectionalLight* directional_light;

    // The array of pointers to point lights.
    gfx::PointLight* point_lights[gfx::MAX_POINT_LIGHTS];

    // An associative array mapping pointers to point lights back to an index into point_lights.
    std::unordered_map<gfx::PointLight*, unsigned int> point_lights_reverse;

    // Initializes the HDR program.
    void InitializeHdrProgram();

    // Initializes the skybox program.
    void InitializeSkyboxProgram();

    // Computes the N Hammersley points where N is defined in constants.h and passes them to the
    // main shader via a uniform array.
    void InitializeHammersleyPoints();

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
    gfx::Vertex PositionToVertex(glm::vec3 position);
};

}
#endif // GFX_GAME_WINDOW_H
