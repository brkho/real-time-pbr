// This is the main entry point of the demo program.
// Brian Ho (brian@brkho.com)

#include "gfx/camera.h"
#include "gfx/color.h"
#include "gfx/directional_light.h"
#include "gfx/environment.h"
#include "gfx/game_window.h"
#include "gfx/model_info.h"
#include "gfx/model_instance.h"
#include "gfx/point_light.h"
#include "gfx/texture_manager.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <exception>
#include <iostream>
#include <vector>

const int kWindowWidth = 1280;
const int kWindowHeight = 800;
const double kRotateSensitivity = 0.005;
const double kPanSensitivity = 0.005;
const double kZoomSensitivity = 0.01;
const std::string kMainVertexShaderPath = "shaders/main.vert";
const std::string kMainFragmentShaderPath = "shaders/main.frag";
const std::string kHdrVertexShaderPath = "shaders/hdr.vert";
const std::string kHdrFragmentShaderPath = "shaders/hdr.frag";
const std::string kSkyboxVertexShaderPath = "shaders/skybox.vert";
const std::string kSkyboxFragmentShaderPath = "shaders/skybox.frag";

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
bool clicking = false;
glm::vec3 pan_offset;

void update_camera() {
  // Convert from spherical coordinates to Cartesian coordinates.
  camera.camera_position = glm::vec3(
      distance * sin(current_pitch) * cos(current_yaw),
      distance * cos(current_pitch),
      distance * sin(current_pitch) * sin(current_yaw));
  camera.camera_position += pan_offset;
  float up_direction = sin(current_pitch) > 0 ? 1.0 : -1.0;
  camera.camera_target = pan_offset;
  camera.camera_up = glm::vec3(0.0f, up_direction, 0.0f);
}

void initialize_camera() {
  current_yaw = 0.0;
  current_pitch = 0.78;
  distance = 3.0;
  pan_offset = glm::vec3(0.0, 0.0, 0.0);
  update_camera();
}

// The callback for a key event that gets regstered with GLFW.
void key_callback(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mode */) {
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
      current_yaw -= (previous_pos.x - x) * kRotateSensitivity;
      current_pitch += (previous_pos.y - y) * kRotateSensitivity;
    } else if (keys[GLFW_KEY_LEFT_CONTROL]) {
      pan_offset += camera.GetRightVector() * (previous_pos.x - x) * kPanSensitivity;
      pan_offset -= camera.GetUpVector() * (previous_pos.y - y) * kPanSensitivity;
    }
    previous_pos.x = x;
    previous_pos.y = y;
  }
}

// Main point of entry for the code.
int main(int /* argc */, char* /* argv */[]) {
  // TODO(brkho): Remove this big try-catch and do actual error handling on a per-line basis.
  try {
    camera = gfx::Camera();
    initialize_camera();
    gfx::GameWindow game_window{kWindowWidth, kWindowHeight, kMainVertexShaderPath,
        kMainFragmentShaderPath, kHdrVertexShaderPath, kHdrFragmentShaderPath,
        kSkyboxVertexShaderPath, kSkyboxFragmentShaderPath, &camera, 45.0f,
        gfx::Color(0.15f, 0.15f, 0.15f)};
    gfx::Environment environment{"assets/hdr/pisa.hdr", 5.0f};

    // gfx::DirectionalLight directional_light = gfx::DirectionalLight(glm::vec3(0.0f, -1.0f, 0.0f),
    //     glm::vec3(2.0f, 2.0f, 2.0f));
    // game_window.SetDirectionalLight(&directional_light);

    gfx::PointLight first_point_light = gfx::PointLight(glm::vec3(8.0f, 8.0f, 16.0f), 1.0f, 0.3f,
        0.04f, glm::vec3(40.0f, 40.0f, 40.0f));
    game_window.AddPointLight(&first_point_light);

    // gfx::PointLight second_point_light = gfx::PointLight(glm::vec3(2.0f, 2.0f, 0.0f), 1.0f, 0.14f,
    //     0.07f, glm::vec3(2.5f, 2.5f, 2.5f));
    // game_window.AddPointLight(&second_point_light);

    gfx::TextureManager texture_manager;

    std::vector<gfx::ModelInstance*> model_instances;

    // gfx::ModelInfo sculpture_info = gfx::ModelInfo("assets/sculpture/sculpture.eo",
    //     &texture_manager, true);
    // gfx::ModelInstance* sculpture_instance = new gfx::ModelInstance(&sculpture_info,
    //     glm::vec3(0.0f, 0.0f, 0.0f));
    // model_instances.push_back(sculpture_instance);

    gfx::ModelInfo drawers_info = gfx::ModelInfo("assets/drawers/drawers.eo",
        &texture_manager, true);
    gfx::ModelInstance* drawers_instance = new gfx::ModelInstance(&drawers_info,
        glm::vec3(0.0f, 0.0f, 0.0f));
    model_instances.push_back(drawers_instance);

    gfx::ModelInfo box_info = gfx::ModelInfo("assets/primitives/box_no_maps.eo", &texture_manager, true);
    box_info.GetMaterial()->albedo_info.value = glm::vec3(1.0, 0.0, 0.0);
    gfx::ModelInstance* box_instance = new gfx::ModelInstance(&box_info,
        glm::vec3(8.0f, 8.0f, 8.0f));
    box_instance->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    box_instance->Update();
    model_instances.push_back(box_instance);

    // for (int x = 0; x < 10; x++) {
    //   for (int y = 0; y < 10; y++) {
    //     gfx::ModelInfo* sphere_info = new gfx::ModelInfo("assets/primitives/sphere_no_maps.eo", &texture_manager, true);
    //     sphere_info->GetMaterial()->albedo_info.value = glm::vec3(0.3, 0.0, 0.0);
    //     sphere_info->GetMaterial()->metallic_info.value = glm::vec3(float(x) / 10.0, 0.0, 0.0);
    //     sphere_info->GetMaterial()->roughness_info.value = glm::vec3(float(y) / 10.0, 0.0, 0.0);
    //     gfx::ModelInstance* sphere_instance = new gfx::ModelInstance(sphere_info, glm::vec3((float)(x - 5) * 2.5, 0.0f, (float)(y - 5) * 2.5));
    //     model_instances.push_back(sphere_instance);
    //   }
    // }

    std::fill_n(keys, 1024, 0);
    glfwSetKeyCallback(game_window.window, key_callback);
    glfwSetMouseButtonCallback(game_window.window, mouse_button_callback);
    glfwSetScrollCallback(game_window.window, scroll_callback);

    double fps_print_time = 2.5;
    double last_time = game_window.GetElapsedTime();

    // Main rendering loop.
    while(game_window.IsRunning()) {
      double current_time = game_window.GetElapsedTime();
      double frame_time = current_time - last_time;
      fps_print_time -= frame_time;
      if (fps_print_time <= 0.0) {
        std::cout << "FPS: " << 1.0 / frame_time << std::endl;
        fps_print_time = 2.5;
      }
      last_time = current_time;

      // Move the light in a circle.
      // float x_pos = -sin(current_time) * 10.0;
      // float y_pos = -cos(current_time) * 10.0;
      // box_instance.position = glm::vec3(x_pos, y_pos, 1.0f);
      // box_instance.Update();

      // first_point_light.position = glm::vec3(x_pos, y_pos, 1.0f);
      // game_window.UpdatePointLight(&first_point_light);

      game_window.PollForEvents();
      handle_input(game_window.window);
      update_camera();

      game_window.PrepareRender(&environment);
      for (gfx::ModelInstance* instance : model_instances) {
        game_window.RenderModel(instance);
      }
      game_window.FinishRender();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
