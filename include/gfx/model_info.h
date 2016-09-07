// This class defines a loaded model made up of one or many meshes with materials. The ModelInfo
// implements the Mappable interface, so it can map its meshes to OpenGL managed buffers.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_MODEL_INFO_H
#define GFX_MODEL_INFO_H

#include "gfx/mappable.h"
#include "gfx/mesh.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace gfx {

class ModelInfo : public gfx::Mappable {
  public:
    // The meshes contained in the ModelInfo.
    std::vector<gfx::Mesh> meshes;

    // TODO: Use a homegrown compact format instead of assimp.
    // Creates a ModelInfo by loading a model with assimp via its path. The should_map argument
    // specifies whether the constructor should map its individual meshes.
    ModelInfo(std::string model_path, bool should_map);

    // Destroys a ModelInfo by calling the destructor on each of its meshes.
    ~ModelInfo();

    // Maps all of the ModelInfo's meshes.
    void Map();

    // Unmaps all of the ModelInfo's meshes.
    void Unmap();

    // Remaps all of the ModelInfo's meshes.
    void Remap();
};

}
#endif // GFX_MODEL_INFO_H
