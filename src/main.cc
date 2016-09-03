// This is the main entry point of the demo program.
// Brian Ho (brian@brkho.com)

#include "game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>

// The callback for a key event that gets regstered with GLFW. Currently this just quits the
// program when the escape key is pressed.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}    

// Main point of entry for the code.
int main(int argc, char * argv[]) {
  // Instantiate the GLFW window.
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
    std::cout << "Failed to create OpenGL context." << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  gladLoadGL();

  // Set callbacks on the newly created window.
  glfwSetKeyCallback(window, key_callback); 

  // Specify the OpenGL viewport size.
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Main rendering loop.
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
