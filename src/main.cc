// This is the main entry point of the demo program.
// Brian Ho (brian@brkho.com)

#include "game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// The callback for a key event that gets regstered with GLFW. Currently this just quits the
// program when the escape key is pressed.
void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mode */) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
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

  // Set callbacks on the newly created window.
  glfwSetKeyCallback(window, key_callback);

  GLfloat vertices[] = {
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
  };
  GLuint indices[] = {
    0, 1, 3,
    1, 2, 3,
  };
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Main rendering loop.
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program); 
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
