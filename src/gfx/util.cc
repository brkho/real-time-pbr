#include "gfx/util.h"

#include <iostream>
#include <string>

GLuint gfx::util::GetSizeOfBuffer(GLenum target) {
  GLint buffer_size = 0;
  glGetBufferParameteriv(target, GL_BUFFER_SIZE, &buffer_size);
  return buffer_size;
}

void gfx::util::GetBufferContents(GLenum target, GLuint* buffer) {
  GLuint size = GetSizeOfBuffer(target);
  glGetBufferSubData(target, 0, size, buffer);
}

void gfx::util::PrettyPrintBuffer(GLenum target) {
  GLuint size = gfx::util::GetSizeOfBuffer(target);
  GLuint *buffer_data = new GLuint[size];
  gfx::util::GetBufferContents(target, buffer_data);
  for (unsigned int i = 0; i < size; ++i) {
    std::cout << buffer_data[i] << ", ";
  }
  std::cout << std::endl;
  delete buffer_data;
}

// Adapted from: https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
void gfx::util::_CheckGlError(const char *file, int line) {
  GLenum error_enum{glGetError()};
  while (error_enum != GL_NO_ERROR) {
    std::string error;
    switch(error_enum) {
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cout << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
    error_enum = glGetError();
  }
}
