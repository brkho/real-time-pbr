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

bool keys[1024];

// The callback for a key event that gets regstered with GLFW. Currently this just quits the
// program when the escape key is pressed.
void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mode */) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  if(action == GLFW_PRESS) {
    keys[key] = true;
  } else if(action == GLFW_RELEASE) {
    keys[key] = false;
  }
}

void perform_movement(glm::vec3* camera_pos, glm::vec3 camera_front, glm::vec3 camera_up) {
  GLfloat camera_speed = 0.01f;
  if(keys[GLFW_KEY_W])
    *camera_pos += camera_speed * camera_front;
  if(keys[GLFW_KEY_S])
    *camera_pos -= camera_speed * camera_front;
  if(keys[GLFW_KEY_A])
    *camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
  if(keys[GLFW_KEY_D])
    *camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;
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

  // Set callbacks on the newly created window.
  std::fill_n(keys, 1024, 0);
  glfwSetKeyCallback(window, key_callback);

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

  // Position attribute.
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // Color attribute.
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);

  // Camera initialization.
  glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f,  3.0f);
  glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 camera_up = glm::vec3(0.0f, 1.0f,  0.0f);

  // Main rendering loop.
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    perform_movement(&camera_pos, camera_front, camera_up);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    glm::mat4 model;
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    GLint model_location = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 view;
    view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
    GLint view_location = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

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

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
