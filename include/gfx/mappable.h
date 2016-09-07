#ifndef GFX_MAPPABLE_H
#define GFX_MAPPABLE_H

namespace gfx {

class Mappable {
  public:
    virtual void Map() = 0;
    virtual void Unmap() = 0;
    virtual void Remap() = 0;
};

}
#endif // GFX_MAPPABLE_H
