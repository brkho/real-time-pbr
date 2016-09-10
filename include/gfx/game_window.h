// // This class defines a game window that abstracts away much of the OpenGL operations. In addition,
// // it will also manage the camera, shaders, and program. The GameWindow will hold references to
// // the Camera and Lights and will require an input of ModelInstances to draw.
// //
// // Brian Ho (brian@brkho.com)

// #ifndef GFX_MODEL_INFO_H
// #define GFX_MODEL_INFO_H

// #include "gfx/mappable.h"
// #include "gfx/mesh.h"

// #include <glad/glad.h>

// #include <string>
// #include <vector>

// namespace gfx {

// class GameWindow {
//   public:
//     // The 
//     std::vector<gfx::Mesh> meshes;

//     // TODO: Use a homegrown compact format instead of assimp.
//     // Creates a ModelInfo by loading a model with assimp via its path. The should_map argument
//     // specifies whether the constructor should map its individual meshes.
//     ModelInfo(std::string model_path, bool should_map);

//     // Copy constructor for ModelInfo that performs a deep copy of the meshes.
//     ModelInfo(const ModelInfo& that);

//     // Move constructor for ModelInfo that moves the meshes into the new ModelInfo.
//     ModelInfo(ModelInfo&& that);

//     // Destroys a ModelInfo by calling the destructor on each of its meshes.
//     ~ModelInfo();

//     // Copies a ModelInfo to another by taking a deep copy of the meshes.
//     ModelInfo& operator= (const ModelInfo& other);

//     // Returns true if all of the ModelInfo's meshes are mapped.
//     bool IsMapped();

//     // Maps all of the ModelInfo's meshes.
//     void Map();

//     // Unmaps all of the ModelInfo's meshes.
//     void Unmap();

//     // Remaps all of the ModelInfo's meshes.
//     void Remap();
//   private:
//     // The shader program used by the GameWindow.
//     GLuint program;
// };

// }
// #endif // GFX_MODEL_INFO_H
