// This class provides a TextureManager for use in materials. The TextureManager stores a mapping
// from file path/name the unique integer handle managed by OpenGL to avoid duplication of texture
// data. While the singleton pattern is not used here to avoid global state, it is unadvisable to
// instantiate more than one TextureManager because this might cause duplications of texture data.

// Brian Ho (brian@dropbox.com)

#ifndef GFX_TEXTURE_MANAGER_H
#define GFX_TEXTURE_MANAGER_H

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace gfx {

class TextureManager {
  public:
    // Default constructor that initializes its members.
    TextureManager();

    // Load the image given by the path argument and set it up as a texture in OpenGL. This also
    // specifies a boolean to convert the sRGB texture into linear space when loading into OpenGL.
    // This returns the OpenGL texture handle associated with the texture.
    GLuint GetTextureHandle(std::string path, bool convert_to_linear);

    // Frees the OpenGL texture data for a given integer handle and updates all Materials that
    // depend on it to point to the null texture instead.
    void FreeTexture(GLuint id);

    // Disable copy constructor and copy assignment.
    TextureManager(TextureManager const&) = delete;
    void operator=(TextureManager const&) = delete;
  private:
    // Hash map from the path to the OpenGL managed texture ID.
    std::unordered_map<std::string, GLuint> path_to_id_map;
};

}
#endif // GFX_TEXTURE_MANAGER_H
