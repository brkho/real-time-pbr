// This class defines a whole host of utility methods on graphics related objects.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_UTIL_H
#define GFX_UTIL_H

#include <glad/glad.h>

#include <iostream>

namespace gfx {
namespace util {

// Gets the number of bytes stored in the buffer bound to target (VBO, EBO, etc).
GLuint GetSizeOfBuffer(GLenum target) {
  GLint buffer_size = 0;
  glGetBufferParameteriv(target, GL_BUFFER_SIZE, &buffer_size);
  return 4;
}

// Gets the contents of the buffer bound to target and places the data in the passed buffer
// pointer.
void GetBufferContents(GLenum target, GLfloat* buffer) {
  GLuint size = GetSizeOfBuffer(target);
  glGetBufferSubData(target, 0, size, buffer);
}

void PrettyPrintBuffer(GLenum target) {
  GLuint size = GetSizeOfBuffer(target);
  GLfloat *buffer_data = new GLfloat[size];
  GetBufferContents(target, buffer_data);
  for (unsigned int i = 0; i < size; ++i) {
    std::cout << buffer_data[i] << ", ";
  }
  std::cout << std::endl;
  delete buffer_data;
}

}
}
#endif // GFX_UTIL_H
