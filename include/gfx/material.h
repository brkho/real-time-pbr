// This class provides an abstraction for materials. A material stores a handle to the diffuse
// texture and a handle to the specular texture (both managed by OpenGL). A material also has a
// specular shininess value. A material can be used with UseMaterial which will bind its textures
// to texture units and set the uniforms accordingly.

// Brian Ho (brian@brkho.com)

#ifndef GFX_MATERIAL_H
#define GFX_MATERIAL_H

#include <glad/glad.h>

#include <string>

namespace gfx {

class Material {
  public:
    // Constructs a material with a path to the diffuse texture, a path to the specular texture,
    // and the shininess as a float..
    Material(std::string diffuse_name, std::string specular_name, GLfloat shininess);

    // Destroys the material by clearing the diffuse texture and the specular texture.
    ~Material();

    // Uses the material for rendering the current model.
    void UseMaterial();

  private:
    // The shininess of the material.
    GLfloat shininess;
    // The OpenGL handle to the diffuse texture.
    GLuint diffuse_handle;
    // The OpenGL handle to the specular texture.
    GLuint specular_handle;
};

}
#endif // GFX_MATERIAL_H
