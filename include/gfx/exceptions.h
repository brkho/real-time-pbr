// This header defines exceptions used in the gfx submodule.

#ifndef GFX_EXCEPTIONS_H
#define GFX_EXCEPTIONS_H

#include <exception>

namespace gfx {

// When the VAO, VBO, or EBO buffers are already mapped and we try to map again.
class BuffersAlreadyMappedException : std::exception {
  public:
    const char * what () const throw () {
      return "Buffers are already mapped.";
    }
};

// When the VAO, VBO, or EBO buffers are not yet mapped and we try to unmap.
class BuffersNotYetMappedException : std::exception {
  public:
    const char * what () const throw () {
      return "Buffers are not yet mapped.";
    }
};

}
#endif // GFX_EXCEPTIONS_H
