#include "gfx/exceptions.h"
#include "gfx/game_window.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

gfx::GameWindow::GameWindow(int width, int height, std::string vertex_path,
    std::string fragment_path, gfx::Camera* camera, float fov, gfx::Color color) : camera{camera},
    window{nullptr}, field_of_view{fov}, program{0}, directional_light{nullptr} {
  gfx::GameWindow::InitializeGameWindow(width, height, color);
  program = gfx::GameWindow::LinkProgram(vertex_path, fragment_path);
  if (program == 0) {
    throw gfx::GameWindowCannotBeInitializedException();
  }
  glUseProgram(program);
}

gfx::GameWindow::GameWindow(int width, int height, std::string vertex_path,
    std::string fragment_path, gfx::Camera* camera) : GameWindow(width, height, vertex_path,
    fragment_path, camera, 45.0f, gfx::Color(0.0f, 0.0f, 0.0f)) {}

bool gfx::GameWindow::IsRunning() {
  return !glfwWindowShouldClose(window);
}

void gfx::GameWindow::InitializeGameWindow(int width, int height, gfx::Color color) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // This is needed to work on OS X.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create the window and make it the current context.
  window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create OpenGL context." << std::endl;
    glfwTerminate();
    throw gfx::GameWindowCannotBeInitializedException();
  }
  glfwMakeContextCurrent(window);
  gladLoadGL();

  glEnable(GL_DEPTH_TEST);
  gfx::GameWindow::SetBufferClearColor(color);
  gfx::GameWindow::UpdateDimensions(width, height);
}

GLuint gfx::GameWindow::CompileShader(std::string path, GLenum shader_type) {
  std::ifstream ifs(path);
  std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  if (content.size() == 0) {
    return 0;
  }

  GLuint shader = glCreateShader(shader_type);
  const char *c_content = content.c_str();
  glShaderSource(shader, 1, &c_content, NULL);
  glCompileShader(shader);

  // Handle errors.
  GLint success;
  GLchar info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if(!success) {
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    std::cout << "Shader compilation of \'" << path << "\' failed:\n" << info_log << std::endl;
    return 0;
  }
  return shader;
}

GLuint gfx::GameWindow::LinkProgram(std::string vertex_path, std::string fragment_path) {
  GLuint vertex_shader = CompileShader(vertex_path, GL_VERTEX_SHADER);
  GLuint frag_shader = CompileShader(fragment_path, GL_FRAGMENT_SHADER);
  if (vertex_shader == 0 || frag_shader == 0) {
    return 0;
  }
  GLuint linked_program = glCreateProgram();
  glAttachShader(linked_program, vertex_shader);
  glAttachShader(linked_program, frag_shader);
  glLinkProgram(linked_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(frag_shader);

  // Handle errors.
  GLint success;
  GLchar info_log[512];
  glGetProgramiv(linked_program, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(linked_program, 512, NULL, info_log);
    std::cout << "Failed to link shader program:\n" << info_log << std::endl;
    return 0;
  }

  if (linked_program == 0) {
    std::cout << "Failed to create shader program." << std::endl;
    glfwTerminate();
    return 0;
  }
  return linked_program;
}

void gfx::GameWindow::SetBufferClearColor(gfx::Color color) {
  glClearColor(color.r, color.g, color.b, color.a);
}

void gfx::GameWindow::SetDirectionalLight(gfx::DirectionalLight* di) {
  directional_light = di;
  GLint di_enabled_location = glGetUniformLocation(program, "directional_light.enabled");
  if (directional_light == nullptr) {
    glUniform1i(di_enabled_location, false);
    return;
  }
  glUniform1i(di_enabled_location, true);
  GLint di_direction_location = glGetUniformLocation(program, "directional_light.direction");
  glUniform3fv(di_direction_location, 1, glm::value_ptr(directional_light->direction));
  GLint di_irradiance_location = glGetUniformLocation(program, "directional_light.irradiance");
  glUniform3fv(di_irradiance_location, 1, glm::value_ptr(directional_light->irradiance));
}

void gfx::GameWindow::UnsetDirectionalLight() {
  SetDirectionalLight(nullptr);
}

void gfx::GameWindow::UpdateDimensions(int width, int height) {
  glfwSetWindowSize(window, width, height);
  int real_width, real_height;
  glfwGetFramebufferSize(window, &real_width, &real_height);
  glViewport(0, 0, real_width, real_height);
  gfx::GameWindow::UpdatePerspectiveProjection(real_width, real_height);
}

void gfx::GameWindow::UpdateFieldOfView(float fov) {
  field_of_view = fov;
  int real_width, real_height;
  glfwGetFramebufferSize(window, &real_width, &real_height);
  gfx::GameWindow::UpdatePerspectiveProjection(real_width, real_height);
}

void gfx::GameWindow::PollForEvents() {
  glfwPollEvents();
}

double gfx::GameWindow::GetElapsedTime() {
  return glfwGetTime();
}

void gfx::GameWindow::PrepareRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GLint view_location = glGetUniformLocation(program, "view_transform");
  glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(camera->GetViewTransform()));
  GLint projection_location = glGetUniformLocation(program, "projection_transform");
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(perspective_projection));
  GLint camera_location = glGetUniformLocation(program, "camera_position");
  glUniform3fv(camera_location, 1, glm::value_ptr(camera->camera_position));
}

void gfx::GameWindow::RenderModel(gfx::ModelInstance* model_instance) {
  model_instance->Draw(program);
}

void gfx::GameWindow::FinishRender() {
  glfwSwapBuffers(window);
}

void gfx::GameWindow::UpdatePerspectiveProjection(int width, int height) {
  perspective_projection = glm::perspective(glm::radians(field_of_view),
      (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}
