#ifndef GFX_MODEL_INSTANCE_H
#define GFX_MODEL_INSTANCE_H

#include "gfx/model_info.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gfx {

class ModelInstance {
  public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat quaternion;

    ModelInstance(std::string model_path, gfx::Color color);
    void Draw();
  private:
    gfx::ModelInfo model_info;
    glm::mat4 model_transform;
    glm::mat4 normal_transform;
};

}
#endif // GFX_MODEL_INSTANCE_H
