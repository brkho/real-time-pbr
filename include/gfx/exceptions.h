// This header defines exceptions used in the gfx submodule.

#ifndef GFX_EXCEPTIONS_H
#define GFX_EXCEPTIONS_H

#include <exception>

namespace gfx {

// When the VAO, VBO, or EBO buffers are already mapped when we expect them to be unmapped.
class BuffersAlreadyMappedException : public std::exception {
  public:
    const char * what () const throw () {
      return "Buffers are already mapped.";
    }
};

// When the VAO, VBO, or EBO buffers are not yet mapped when we expect them to be mapped.
class BuffersNotYetMappedException : public std::exception {
  public:
    const char * what () const throw () {
      return "Buffers are not yet mapped.";
    }
};

// When Assimp cannot read a file.
class AssimpInvalidFileException : public std::exception {
  public:
    const char * what () const throw () {
      return "Assimp cannot read the file.";
    }
};

// When the game window cannot be initialized.
class GameWindowCannotBeInitializedException : public std::exception {
  public:
    const char * what () const throw () {
      return "Game window cannot be initialized.";
    }
};

// When the texture manager cannot read the image.
class CannotLoadTextureException : public std::exception {
  public:
    const char * what () const throw () {
      return "Texture cannot be loaded.";
    }
};

// When there are too many lights in the scene (over gfx::MAX_POINT_LIGHTS).
class TooManyLightsException : public std::exception {
  public:
    const char * what () const throw () {
      return "Too many lights in the scene.";
    }
};

// When there is an invalid access of the lights array.
class InvalidLightException : public std::exception {
  public:
    const char * what () const throw () {
      return "Invalid access of light array.";
    }
};

}
#endif // GFX_EXCEPTIONS_H
