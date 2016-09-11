// This class describes a generic Color that stores the RGBA components as GLfloats internally.
// It provides constructors to create with float RGBA, float RGB, uint RGBA, and uint RGB.

// Brian Ho (brian@brkho.com)

#ifndef GFX_COLOR_H
#define GFX_COLOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gfx {

class Color {
  public:
    // The red component.
    GLfloat r;
    // The green component.
    GLfloat g;
    // The blue component.
    GLfloat b;
    // The alpha component.
    GLfloat a;

    // Create a Color with RGBA values in the floating point range 0.0-1.0.
    Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) :
      r{r}, g{g}, b{b}, a{a} {}

    // Create a Color with RGB values in the floating point range 0.0-1.0. The value of the alpha
    // channel is set to 1.0.
    Color(GLfloat r, GLfloat g, GLfloat b) :
      r{r}, g{g}, b{b}, a{1.0f} {}

    // Create a Color with RGBA values in the uint range 0-255.
    Color(GLuint r, GLuint g, GLuint b, GLuint a) :
      r{(GLfloat)r / 255.0f}, g{(GLfloat)g / 255.0f}, b{(GLfloat)b / 255.0f},
      a{(GLfloat)a / 255.0f} {}

    // Create a Color with RGBA values in the uint range 0-255. The value of the alpha channel is
    // set to 255.
    Color(GLuint r, GLuint g, GLuint b) :
      r{(GLfloat)r / 255.0f}, g{(GLfloat)g / 255.0f}, b{(GLfloat)b / 255.0f}, a{1.0f} {}

    // Converts a color to a glm::vec4.
    glm::vec3 ToVec4() {
      return glm::vec4(r, g, b, a);
    }
};

}
#endif // GFX_COLOR_H
