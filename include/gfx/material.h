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
    Material(std::string diffuse_name, std::string specular_name, GLfloat shininess);
    void UseMaterial();

  private:
    GLfloat shininess;
    GLuint diffuse_handle;
    GLuint specular_handle;
};

}
#endif // GFX_MATERIAL_H
