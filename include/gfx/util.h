// This class defines a whole host of utility methods on graphics related objects.
//
// Brian Ho (brian@brkho.com)

#ifndef GFX_UTIL_H
#define GFX_UTIL_H

#include <glad/glad.h>

namespace gfx {
namespace util {

// Gets the number of bytes stored in the buffer bound to target (VBO, EBO, etc).
GLuint GetSizeOfBuffer(GLenum target);

// Gets the contents of the buffer bound to target and places the data in the passed buffer
// pointer.
void GetBufferContents(GLenum target, GLuint* buffer);

// Pretty prints the contents of a buffer bound to target.
void PrettyPrintBuffer(GLenum target);

// Checks the current errors queued up in OpenGL and prints it to standard output.
void _CheckGlError(const char *file, int line);
#define CheckGlError() _CheckGlError(__FILE__, __LINE__)

}
}
#endif // GFX_UTIL_H
