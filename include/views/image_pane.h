#pragma once
#include <bgfx/bgfx.h>
#include "views/view.h"

namespace views {

class ImagePane : public View {
  bgfx::TextureHandle texture;
  bgfx::VertexBufferHandle vertexBuffer;
  bgfx::IndexBufferHandle indexBuffer;
  bgfx::ProgramHandle program;
  bgfx::UniformHandle textureColor;

public:
  ImagePane(const std::string& path);
  ~ImagePane();
  void setImage(const std::string& path);
  void render() const;
};
}
