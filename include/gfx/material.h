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
    // The shininess of the material.
    GLfloat shininess;
    // The ambient lighting coefficient.
    GLfloat ambient_coefficient;

    // Constructs a material with an OpenGL handle to a diffuse texture, an OpenGL handle to a
    // specular texture, an OpenGL handle to a normal map, the shininess as a float, and an ambient
    // lighting coefficient. Use handles of 0 to use no texture.
    Material(GLuint diffuse_handle, GLuint specular_handle,
        GLfloat shininess, GLfloat ambient);

    // Constructs a material with an OpenGL handle to a diffuse texture, an OpenGL handle to a
    // specular texture, an OpenGL handle to a normal map, and the shininess as a float. Use
    // handles of 0 to use no texture. This initializes the ambient lighting coefficient to 0.03
    // and the base color to white.
    Material(GLuint diffuse_handle, GLuint specular_handle,
        GLfloat shininess);

    // Destroys the material by clearing the diffuse texture and the specular texture.
    ~Material();

    // Uses the material for rendering the current model given a handle to the shader program.
    void UseMaterial(GLuint program);

    // Unregisters the given texture handle from any of the material's texture handles.
    void RemoveTexture(GLuint id);

  private:
    // The OpenGL handle to the diffuse texture.
    GLuint diffuse_handle;
    // The OpenGL handle to the specular texture.
    GLuint specular_handle;
};

}
#endif // GFX_MATERIAL_H
