// This is the main entry point of the demo program.
// Brian Ho (brian@brkho.com)

#include "game.h"
#include "gfx/model_info.h"
#include "gfx/model_instance.h"
#include "gfx/exceptions.h"
#include "gfx/util.h"
#include "gfx/camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>

struct Position {
  double x;
  double y;
};

gfx::Camera camera;

Position previous_pos;
double current_yaw;
double current_pitch;
double distance;
bool keys[1024];
bool clicking;
glm::vec3 pan_offset;

void update_camera() {
  // Convert from spherical coordinates to Cartesian coordinates.
  camera.camera_position = glm::vec3(
      distance * sin(current_pitch) * cos(current_yaw),
      distance * sin(current_pitch) * sin(current_yaw),
      distance * cos(current_pitch));
  camera.camera_position += pan_offset;
  float up_direction = sin(current_pitch) > 0 ? 1.0 : -1.0;
  camera.camera_target = pan_offset;
  camera.camera_up = glm::vec3(0.0f, 0.0f, up_direction);
}

void initialize_camera() {
  current_yaw = 0.0;
  current_pitch = 0.78;
  distance = 3.0;
  pan_offset = glm::vec3(0.0, 0.0, 0.0);
  update_camera();
}

// The callback for a key event that gets regstered with GLFW.
void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mode */) {
  if(action == GLFW_PRESS) {
    keys[key] = true;
  } else if(action == GLFW_RELEASE) {
    keys[key] = false;
  }
}

// The callback for a mouse click that gets registered with GLFW.
void mouse_button_callback(GLFWwindow* window, int button, int action, int /* mods */) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    previous_pos.x = x;
    previous_pos.y = y;
    clicking = action == GLFW_PRESS;
  }
}

// The callback for a mouse scroll that gets registered with GLFW.
void scroll_callback(GLFWwindow* /* window */, double /* x */, double y) {
  distance = std::max(0.1, distance - y * kZoomSensitivity);
}

void handle_input(GLFWwindow* window) {
  if (keys[GLFW_KEY_ESCAPE]) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else if (keys[GLFW_KEY_F]){
    initialize_camera();
  }
  if (clicking) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    if (keys[GLFW_KEY_LEFT_ALT]) {
      current_yaw += (previous_pos.x - x) * kRotateSensitivity;
      current_pitch += (previous_pos.y - y) * kRotateSensitivity;
    } else if (keys[GLFW_KEY_LEFT_CONTROL]) {
      pan_offset += camera.GetRightVector() * (previous_pos.x - x) * kPanSensitivity;
      pan_offset -= camera.GetUpVector() * (previous_pos.y - y) * kPanSensitivity;
    }
    previous_pos.x = x;
    previous_pos.y = y;
  }
}

// Sets up gl3w and returns the GLFW context. If the context cannot be set up, return nullptr.
GLFWwindow* SetupGl3wAndGlfwContext() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // This is needed to work on OS X.
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create the window and make it the current context.
  GLFWwindow* window = glfwCreateWindow(kWindowWidth, kWindowHeight, "OpenGL", nullptr, nullptr);
  if (window == nullptr) {
    return nullptr;
  }
  glfwMakeContextCurrent(window);
  gladLoadGL();

  // Specify the OpenGL viewport size.
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  return window;
}

// Compiles a shader given a path to the source and the shader type and returns a reference to that
// shader or 0 if unsuccessful.
GLuint CompileShader(std::string path, GLenum shader_type) {
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

// Takes a path to a vertex shader and a fragment shader, compiles them both, and links them
// together into a shader program. This returns a reference to the shader program if successful or
// 0 if unsuccessful.
GLuint LinkProgram(std::string vertex_path, std::string frag_path) {
  GLuint vertex_shader = CompileShader(vertex_path, GL_VERTEX_SHADER);
  GLuint frag_shader = CompileShader(frag_path, GL_FRAGMENT_SHADER);
  if (vertex_shader == 0 || frag_shader == 0) {
    return 0;
  }
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, frag_shader);
  glLinkProgram(program);
  glDeleteShader(vertex_shader);
  glDeleteShader(frag_shader);

  // Handle errors.
  GLint success;
  GLchar info_log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);  
  if(!success) {
    glGetProgramInfoLog(program, 512, NULL, info_log);
    std::cout << "Failed to link shader program:\n" << info_log << std::endl;
    return 0;
  }
  return program;
}

// Main point of entry for the code.
int main(int /* argc */, char* /* argv */[]) {
  GLFWwindow* window = SetupGl3wAndGlfwContext();
  if (window == nullptr) {
    std::cout << "Failed to create OpenGL context." << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  GLuint program = LinkProgram("shaders/std.vert", "shaders/std.frag");
  if (program == 0) {
    std::cout << "Failed to create shader program." << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glUseProgram(program);

  // Set callbacks on the newly created window.
  std::fill_n(keys, 1024, 0);
  clicking = false;
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  GLfloat vertices[] = {
    // Positions         // Colors
    -1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,  1.0f, 1.0f, 1.0f,

  };
  GLuint indices[] = {
    0, 1, 2,
    2, 3, 0,
  };
  // GLuint vao;
  // GLuint vbo;
  // GLuint ebo;
  // glGenVertexArrays(1, &vao);
  // glBindVertexArray(vao);
  // glGenBuffers(1, &vbo);
  // glGenBuffers(1, &ebo);
  // glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // // Position attribute.
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  // glEnableVertexAttribArray(0);
  // // Color attribute.
  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
  // glEnableVertexAttribArray(1);

  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);


  camera = gfx::Camera();



  initialize_camera();


  // gfx::Vertex vertex{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)};
  // std::vector<gfx::Vertex> test_vertices {vertex};
  // std::vector<GLuint> test_indices {1, 2, 3, 4};
  // gfx::ModelInfo model_info {test_vertices, test_indices};
  // model_info.Draw();


  gfx::ModelInfo model_info = gfx::ModelInfo("assets/sphere.obj", true);
  gfx::ModelInstance model_instance = gfx::ModelInstance(&model_info);



  // Main rendering loop.
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    handle_input(window);
    update_camera();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    // glm::mat4 model;
    // model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // GLint model_location = glGetUniformLocation(program, "model");
    // glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

    GLint view_location = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(camera.GetViewTransform()));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (GLfloat)kWindowWidth / kWindowHeight,
        0.1f, 100.0f);
    GLint projection_location = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

    // glm::mat4 transform;
    // transform = glm::rotate(transform, glm::radians((GLfloat)glfwGetTime() * 50.0f), glm::vec3(0.0, 0.0, 1.0));
    // transform = glm::scale(transform, glm::vec3(0.5, 0.5, 0.5));  
    // GLint transform_location = glGetUniformLocation(program, "transform");
    // glUniformMatrix4fv(transform_location, 1, GL_FALSE, glm::value_ptr(transform));

    // glBindVertexArray(vao);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    model_instance.Draw(program);
    gfx::util::CheckGlError();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
