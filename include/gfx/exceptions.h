// This header defines exceptions used in the gfx submodule.

#ifndef GFX_EXCEPTIONS_H
#define GFX_EXCEPTIONS_H

#include <exception>

namespace gfx {

// When the VAO, VBO, or EBO buffers are already mapped when we expect them to be unmapped.
class BuffersAlreadyMappedException : std::exception {
  public:
    const char * what () const throw () {
      return "Buffers are already mapped.";
    }
};

// When the VAO, VBO, or EBO buffers are not yet mapped when we expect them to be mapped.
class BuffersNotYetMappedException : std::exception {
  public:
    const char * what () const throw () {
      return "Buffers are not yet mapped.";
    }
};

// When Assimp cannot read a file.
class AssimpInvalidFileException : std::exception {
  public:
    const char * what () const throw () {
      return "Assimp cannot read the file.";
    }
};

// When the game window cannot be initialized.
class GameWindowCannotBeInitializedException : std::exception {
  public:
    const char * what () const throw () {
      return "Game window cannot be initialized.";
    }
};

}
#endif // GFX_EXCEPTIONS_H
