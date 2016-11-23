// This class provides an abstraction for materials. A material stores handles to many material
// maps managed by OpenGL. A material also has an ambient lighting value. A material can be used
// with UseMaterial which will bind its textures to texture units and set the uniforms accordingly.

// Brian Ho (brian@brkho.com)

#ifndef GFX_MATERIAL_H
#define GFX_MATERIAL_H

#include <glad/glad.h>

#include <string>

namespace gfx {

enum ShaderType { BlinnPhong, CookTorrance, AshikhminShirley };

class Material {
  public:
    // The ambient lighting coefficient.
    GLfloat ambient_coefficient;

    // Constructs a material with a shader type, an OpenGL handle to an albedo map, an OpenGL
    // handle to a specular map, an OpenGL handle to a gloss map, an OpenGL handle to an IOR map,
    // an OpenGL handle to a normal map, an OpenGL handle to an AO map, and an ambient lighting
    // coefficient. Use handles of 0 to use no map. If no map is specified (by using a handle of
    // 0), these defaults are used:
    // albedo: all white
    // specular: no specular reflection (all black)
    // gloss: TBD
    // IOR: TBD
    // normal: unmodified model normals (all (127, 127, 127))
    Material(ShaderType shader_type, GLuint albedo_handle, GLuint specular_handle,
        GLuint gloss_handle, GLuint ior_handle, GLuint normal_handle, GLuint ao_handle,
        GLfloat ambient);

    // Destroys the material by clearing the material maps.
    ~Material();

    // Uses the material for rendering the current model given a handle to the shader program.
    void UseMaterial(GLuint program);

    // Unregisters the given texture handle from any of the material's texture handles.
    void RemoveTexture(GLuint id);

  private:
    // The shader type used to render models with the material.
    ShaderType shader_type;
    // The OpenGL handle to the albedo map.
    GLuint albedo_handle;
    // The OpenGL handle to the specular map.
    GLuint specular_handle;
    // The OpenGL handle to the specular map.
    GLuint gloss_handle;
    // The OpenGL handle to the specular map.
    GLuint ior_handle;
    // The OpenGL handle to the normal map.
    GLuint normal_handle;
    // The OpenGL handle to the ambient occlusion map.
    GLuint ao_handle;
};

}
#endif // GFX_MATERIAL_H
