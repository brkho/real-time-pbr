#include "gfx/exceptions.h"
#include "gfx/game_window.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

gfx::GameWindow::GameWindow(int width, int height, std::string main_vertex_path,
    std::string main_fragment_path, std::string hdr_vertex_path, std::string hdr_fragment_path,
    gfx::Camera* camera, float fov, gfx::Color color) : camera{camera}, window{nullptr},
    field_of_view{fov}, program{0}, vp_width{0}, vp_height{0}, hdr_program{0},
    multisampled_hdr_fbo{0}, singlesampled_hdr_fbo{0}, hdr_color_buffer{0}, matrix_handle{0},
    draw_quad{nullptr}, quad_vertices{nullptr}, quad_elements{nullptr}, directional_light{nullptr} {
  for (unsigned int i = 0; i < gfx::MAX_POINT_LIGHTS; i++) {
    point_lights[i] = nullptr;
  }
  gfx::GameWindow::InitializeGameWindow(width, height, color);
  program = gfx::GameWindow::LinkProgram(main_vertex_path, main_fragment_path);
  hdr_program = gfx::GameWindow::LinkProgram(hdr_vertex_path, hdr_fragment_path);
  if (program == 0 || hdr_program == 0) {
    throw gfx::GameWindowCannotBeInitializedException();
  }

  // TODO(brkho): Implement resizing.
  GLint dimensions[4];
  glGetIntegerv(GL_VIEWPORT, dimensions);
  vp_width = dimensions[2];
  vp_height = dimensions[3];
  glUseProgram(program);

  // TODO(brkho): Actually save the handle to the multisampled HDR color buffer and the depth render
  // buffer object so we can clean them up on GameWindow destruction.
  // Generate the intermediate framebuffer which stores the HDR output.
  glGenFramebuffers(1, &multisampled_hdr_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, multisampled_hdr_fbo);
  GLuint multisampled_hdr_color_buffer;
  glGenTextures(1, &multisampled_hdr_color_buffer);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampled_hdr_color_buffer);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, gfx::MSAA_SAMPLES, GL_RGBA16F, vp_width,
      vp_height, GL_TRUE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
      multisampled_hdr_color_buffer, 0);
  GLuint depth_rbo;
  glGenRenderbuffers(1, &depth_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, gfx::MSAA_SAMPLES, GL_DEPTH_COMPONENT,
      vp_width, vp_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

  // Set up a non-multisampled FBO to blit from the multisampled FBO. I don't think this needs a
  // depth buffer since it's only ever used to convert from multisampled to singlesampled texture.
  glGenFramebuffers(1, &singlesampled_hdr_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, singlesampled_hdr_fbo);
  glGenTextures(1, &hdr_color_buffer);
  glBindTexture(GL_TEXTURE_2D, hdr_color_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, vp_width, vp_height, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color_buffer, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // Set up quad used for rendering the output texture.
  glUseProgram(hdr_program);
  quad_vertices = new std::vector<gfx::Vertex>();
  quad_vertices->push_back(gfx::GameWindow::PositionToQuadVertex(glm::vec3(-1.0f, -1.0f, 0.0f)));
  quad_vertices->push_back(gfx::GameWindow::PositionToQuadVertex(glm::vec3(-1.0f, 1.0f, 0.0f)));
  quad_vertices->push_back(gfx::GameWindow::PositionToQuadVertex(glm::vec3(1.0f, -1.0f, 0.0f)));
  quad_vertices->push_back(gfx::GameWindow::PositionToQuadVertex(glm::vec3(1.0f, 1.9f, 0.0f)));
  quad_elements = new std::vector<GLuint>{0, 1, 3, 3, 2, 0};
  draw_quad = new gfx::Mesh(quad_vertices, quad_elements, nullptr, true);

  // Set up the dithering texture to combat banding in low lighting conditions.
  glGenTextures(1, &matrix_handle);
  glBindTexture(GL_TEXTURE_2D, matrix_handle);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 8, 8, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, bayer_matrix);
  glUniform1i(glGetUniformLocation(hdr_program, "hdrBuffer"), 0);
  glUniform2ui(glGetUniformLocation(hdr_program, "dimensions"), vp_width, vp_height);
  glUniform1i(glGetUniformLocation(hdr_program, "bayer_matrix"), 1);

  glUseProgram(program);
}

gfx::GameWindow::GameWindow(int width, int height, std::string main_vertex_path,
    std::string main_fragment_path, std::string hdr_vertex_path, std::string hdr_fragment_path,
    gfx::Camera* camera) : GameWindow(width, height, main_vertex_path, main_fragment_path,
    hdr_vertex_path, hdr_fragment_path, camera, 45.0f, gfx::Color(0.0f, 0.0f, 0.0f)) {}

gfx::Vertex gfx::GameWindow::PositionToQuadVertex(glm::vec3 position) {
  return gfx::Vertex{position, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
      glm::vec2{0.0f, 0.0f}};
}

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
  glEnable(GL_MULTISAMPLE);

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
  if (directional_light == nullptr) {
    GLint di_enabled_location = glGetUniformLocation(program, "directional_light.enabled");
    glUniform1i(di_enabled_location, false);
    return;
  }
  UpdateDirectionalLight();
}

void gfx::GameWindow::UpdateDirectionalLight() {
  GLint di_enabled_location = glGetUniformLocation(program, "directional_light.enabled");
  glUniform1i(di_enabled_location, true);
  GLint di_direction_location = glGetUniformLocation(program, "directional_light.direction");
  glm::vec3 normalized_direction = glm::normalize(directional_light->direction);
  glUniform3fv(di_direction_location, 1, glm::value_ptr(normalized_direction));
  GLint di_irradiance_location = glGetUniformLocation(program, "directional_light.irradiance");
  glUniform3fv(di_irradiance_location, 1, glm::value_ptr(directional_light->irradiance));
}

void gfx::GameWindow::UnsetDirectionalLight() {
  SetDirectionalLight(nullptr);
}

unsigned int gfx::GameWindow::GetAndValidatePointLightIndex(gfx::PointLight* point_light) {
  auto reverse_it = point_lights_reverse.find(point_light);
  if (reverse_it == point_lights_reverse.end()) {
    throw gfx::InvalidLightException();
  }
  return reverse_it->second;
}

void gfx::GameWindow::AddPointLight(gfx::PointLight* point_light) {
  // Trying to add the same point light twice.
  auto reverse_it = point_lights_reverse.find(point_light);
  if (reverse_it != point_lights_reverse.end()) {
    throw gfx::InvalidLightException();
  }

  int free_index = -1;
  for (unsigned int i = 0; i < gfx::MAX_POINT_LIGHTS; i++) {
    if (point_lights[i] == nullptr) {
      free_index = i;
      break;
    }
  }
  if (free_index == -1) {
    throw gfx::TooManyLightsException();
  }
  point_lights_reverse[point_light] = free_index;
  point_lights[free_index] = point_light;
  UpdatePointLight(point_light);
}

void gfx::GameWindow::RemovePointLight(gfx::PointLight* point_light) {
  unsigned int index = GetAndValidatePointLightIndex(point_light);
  std::string light_base = "point_lights[" + std::to_string(index) + "].";
  GLint enabled_location = glGetUniformLocation(program, (light_base + "enabled").c_str());
  glUniform1i(enabled_location, false);
  point_lights_reverse.erase(point_light);
  point_lights[index] = nullptr;
}

void gfx::GameWindow::UpdatePointLight(gfx::PointLight* point_light) {
  unsigned int index = GetAndValidatePointLightIndex(point_light);
  std::string light_base = "point_lights[" + std::to_string(index) + "].";
  GLint enabled_location = glGetUniformLocation(program, (light_base + "enabled").c_str());
  glUniform1i(enabled_location, true);
  GLint position_location = glGetUniformLocation(program, (light_base + "position").c_str());
  glUniform3fv(position_location, 1, glm::value_ptr(point_lights[index]->position));
  GLint irradiance_location = glGetUniformLocation(program, (light_base + "irradiance").c_str());
  glUniform3fv(irradiance_location, 1, glm::value_ptr(point_lights[index]->irradiance));
  GLint const_atten_location = glGetUniformLocation(program, (light_base + "const_atten").c_str());
  glUniform1f(const_atten_location, point_lights[index]->const_atten);
  GLint lin_atten_location = glGetUniformLocation(program, (light_base + "linear_atten").c_str());
  glUniform1f(lin_atten_location, point_lights[index]->linear_atten);
  GLint quad_atten_location = glGetUniformLocation(program, (light_base + "quad_atten").c_str());
  glUniform1f(quad_atten_location, point_lights[index]->quad_atten);
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
  glUseProgram(program);
  glBindFramebuffer(GL_FRAMEBUFFER, multisampled_hdr_fbo);
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
  glUseProgram(hdr_program);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr_color_buffer);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, matrix_handle);

  // Blit from the multisampled FBO to the singlesampled FBO.
  // TODO(brkho): Investigate whether it's better to blit from a renderbuffer to a texture.
  glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampled_hdr_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, singlesampled_hdr_fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlitFramebuffer(0, 0, vp_width, vp_height, 0, 0, vp_width, vp_height, GL_COLOR_BUFFER_BIT,
      GL_NEAREST);

  // Bind the default frame buffer, so we can actually render to the screen.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render a quad that covers the entire screen so we can actually use the texture produced by the
  // main shader.
  glBindVertexArray(draw_quad->vao);
  glDrawElements(GL_TRIANGLES, draw_quad->GetNumberOfIndices(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glfwSwapBuffers(window);
}

void gfx::GameWindow::UpdatePerspectiveProjection(int width, int height) {
  perspective_projection = glm::perspective(glm::radians(field_of_view),
      (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}
