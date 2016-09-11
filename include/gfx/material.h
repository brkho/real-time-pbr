// This class provides an abstraction for materials. A material stores a handle to the diffuse
// texture and a handle to the specular texture (both managed by OpenGL). A material also has a
// specular shininess value. A material can be used with UseMaterial which will bind its textures
// to texture units and set the uniforms accordingly.

// Brian Ho (brian@brkho.com)

#ifndef GFX_MATERIAL_H
#define GFX_MATERIAL_H

#include "gfx/color.h"

#include <glad/glad.h>

#include <string>

namespace gfx {

class Material {
  public:
    // Constructs a material with a path to the diffuse texture, a path to the specular texture,
    // a base color, and the shininess as a float, and an ambient lighting coefficient.
    Material(std::string diffuse_name, std::string specular_name, gfx::Color color,
        GLfloat shininess, GLfloat ambient);

    // Constructs a material with a path to the diffuse texture, a path to the specular texture,
    // and the shininess as a float. This initializes the ambient lighting coefficient to 0.03 and
    // the base color to white.
    Material(std::string diffuse_name, std::string specular_name, GLfloat shininess);

    // Destroys the material by clearing the diffuse texture and the specular texture.
    ~Material();

    // Uses the material for rendering the current model given a handle to the shader program.
    void UseMaterial(GLuint program);

  private:
    // The base color of the material.
    gfx::Color color;
    // The shininess of the material.
    GLfloat shininess;
    // The OpenGL handle to the diffuse texture.
    GLuint diffuse_handle;
    // The OpenGL handle to the specular texture.
    GLuint specular_handle;
    // The ambient lighting coefficient.
    GLfloat ambient_coefficient;
};

}
#endif // GFX_MATERIAL_H
