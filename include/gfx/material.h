// This class provides an abstraction for materials. A material stores handles to many material
// maps managed by OpenGL. A material also has an ambient lighting value. A material can be used
// with UseMaterial which will bind its textures to texture units and set the uniforms accordingly.

// Brian Ho (brian@brkho.com)

#ifndef GFX_MATERIAL_H
#define GFX_MATERIAL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace gfx {

enum ShaderType { BlinnPhong, CookTorrance, AshikhminShirley };

// Describes a map to be used in the material.
struct MapInfo {
  // OpenGL handle to the map. This will be 0 if no map is to be used.
  GLuint handle;

  // The value to use for the entire mesh if no map is used (handle = 0).
  glm::vec3 value;

  // Default constructor.
  MapInfo(GLuint handle, glm::vec3 value) : handle{handle}, value{value} {}

  // Copy constructor.
  MapInfo(const MapInfo &other) : handle{other.handle}, value{other.value} {}
};

class Material {
  public:
    // The ambient lighting coefficient.
    GLfloat ambient_coefficient;
    // Info for the albedo map.
    MapInfo albedo_info;
    // Info for the roughness map.
    MapInfo metallic_info;
    // Info for the roughness map.
    MapInfo roughness_info;
    // Info for the normal map.
    MapInfo normal_info;
    // Info for the ambient occlusion map.
    MapInfo ao_info;

    // Constructs a material with a shader type, an OpenGL handle to an albedo map, an OpenGL
    // handle to a roughness map, an OpenGL handle to a normal map, an OpenGL handle to an AO map,
    // and an ambient lighting coefficient. Use handles of 0 to use no map. If no map is specified
    // (by using a handle of 0), these defaults are used:
    // albedo: vec3(1.0, 1.0, 1.0)
    // roughness: vec3(0.5, 0.5, 0.5)
    // normal: unmodified model normals (all (0.5, 0.5, 1.0))
    // ao: no ao vec4(1.0, 1.0 ,1.0)
    Material(ShaderType shader_type, MapInfo albedo_info, MapInfo roughness_info,
        MapInfo metallic_info, MapInfo normal_info, MapInfo ao_info, GLfloat ambient);

    // Destroys the material by clearing the material maps.
    ~Material();

    // Uses the material for rendering the current model given a handle to the shader program.
    void UseMaterial(GLuint program);

    // Unregisters the given texture handle from any of the material's texture handles.
    void RemoveTexture(GLuint id);

  private:
    // The shader type used to render models with the material.
    ShaderType shader_type;

    // Binds a map given a program, a MapInfo, a string with the map type, and a Texture Unit
    // number.
    void BindMap(GLuint program, MapInfo info, std::string map_type, GLuint texture_unit);
};

}
#endif // GFX_MATERIAL_H
