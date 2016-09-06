// This class describes a generic Color that stores the RGBA components as GLfloats internally.
// It provides constructors to create with float RGBA, float RGB, uint RGBA, and uint RGB.

// Brian Ho (brian@brkho.com)

#ifndef GFX_COLOR_H
#define GFX_COLOR_H

#include <glad/glad.h>

namespace gfx {

class Color {
  public:
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;

    Color(GLfloat r, GLfloat g, GLfloat b, GLfloat a) :
      r{r}, g{g}, b{b}, a{a} {}
    Color(GLfloat r, GLfloat g, GLfloat b) :
      r{r}, g{g}, b{b}, a{1.0f} {}
    Color(GLuint r, GLuint g, GLuint b, GLuint a) :
      r{(GLfloat)r / 255.0f}, g{(GLfloat)g / 255.0f}, b{(GLfloat)b / 255.0f},
      a{(GLfloat)a / 255.0f} {}
    Color(GLuint r, GLuint g, GLuint b) :
      r{(GLfloat)r / 255.0f}, g{(GLfloat)g / 255.0f}, b{(GLfloat)b / 255.0f} {}
};

}
#endif // GFX_COLOR_H
