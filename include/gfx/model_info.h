// This class defines a loaded model made up of one or many meshes with materials. The ModelInfo
// implements the Mappable interface, so it can map its meshes to OpenGL managed buffers.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_MODEL_INFO_H
#define GFX_MODEL_INFO_H

#include "gfx/mappable.h"
#include "gfx/material.h"
#include "gfx/mesh.h"
#include "gfx/texture_manager.h"

#include <glad/glad.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace gfx {

// The base path for assets in the engine.
const std::string assets_path = "assets";
// Maps the shader type from the .eo object to an enum value.
const std::vector<gfx::ShaderType> shader_map {gfx::BlinnPhong, gfx::CookTorrance,
    gfx::AshikhminShirley};

class ModelInfo : public gfx::Mappable {
  public:
    // The meshes contained in the ModelInfo.
    std::vector<gfx::Mesh> meshes;

    // Creates a ModelInfo by loading an EO format model via its path and a TextureManage. The
    // should_map argument specifies whether the constructor should map its individual meshes.
    ModelInfo(std::string model_path, gfx::TextureManager* manager, bool should_map);

    // Copy constructor for ModelInfo that performs a deep copy of the meshes.
    ModelInfo(const ModelInfo& that);

    // Move constructor for ModelInfo that moves the meshes into the new ModelInfo.
    ModelInfo(ModelInfo&& that);

    // Destroys a ModelInfo by calling the destructor on each of its meshes.
    ~ModelInfo();

    // Copies a ModelInfo to another by taking a deep copy of the meshes.
    ModelInfo& operator= (const ModelInfo& other);

    // Returns true if all of the ModelInfo's meshes are mapped.
    bool IsMapped();

    // Maps all of the ModelInfo's meshes.
    void Map();

    // Unmaps all of the ModelInfo's meshes.
    void Unmap();

    // Remaps all of the ModelInfo's meshes.
    void Remap();

    // Returns a shared_ptr to the material.
    std::shared_ptr<gfx::Material> GetMaterial();
  private:
    // Loads the next material map path in the EO model stream given a TextureManager. This loads
    // the map from file into OpenGL managed memory and returns a handle to the texture. This also
    // takes a boolean specifying whether we should gamma correct the sRGB texture into linear
    // space when loading into OpenGL.
    GLuint LoadMap(std::ifstream* input_file, gfx::TextureManager* manager, bool convert_to_linear);
};

}
#endif // GFX_MODEL_INFO_H
