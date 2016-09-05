#include "gfx/model_info.h"

#include <iostream>

gfx::ModelInfo::ModelInfo(std::vector<gfx::Vertex> vertices, std::vector<GLuint> indices) :
    vertices{vertices}, indices{indices} {
}

void gfx::ModelInfo::Draw() {
  std::cout << "vertices: " << vertices.size() << std::endl;
}
