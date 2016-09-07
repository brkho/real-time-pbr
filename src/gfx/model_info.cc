#include "gfx/model_info.h"

#include <iostream>

gfx::ModelInfo::ModelInfo(std::string model_path, bool should_map) :
    meshes{std::vector<gfx::Mesh>()} {
  return;
}

gfx::ModelInfo::~ModelInfo() {
  return;
}

void gfx::ModelInfo::Map() {
  return;
}

void gfx::ModelInfo::Unmap() {
  return;
}

void gfx::ModelInfo::Remap() {
  gfx::ModelInfo::Unmap();
  gfx::ModelInfo::Map();
}