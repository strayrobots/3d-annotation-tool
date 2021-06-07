#include <bgfx/bgfx.h>
#include "shader_utils.h"
#include "texture_utils.h"
#include "views/image_pane.h"
#include "views/view.h"

namespace views {

static const float vertices[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f};

static const uint16_t indices[] = {
    0, 3, 1,
    0, 2, 3};
ImagePane::ImagePane(const std::string& path, int viewId) : View(viewId) {
  bgfx::VertexLayout layout;
  layout.begin()
      .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
      .end();
  vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), layout);
  indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
  texture = texture_utils::loadTexture(path);
  textureColor = bgfx::createUniform("textureColor", bgfx::UniformType::Sampler);
  program = shader_utils::loadProgram("vs_image_pane", "fs_image_pane");
}

ImagePane::~ImagePane() {
  bgfx::destroy(program);
  bgfx::destroy(vertexBuffer);
  bgfx::destroy(indexBuffer);
  bgfx::destroy(textureColor);
  bgfx::destroy(texture);
}

void ImagePane::setImage(const std::string& path) {
  bgfx::destroy(texture);
  texture = texture_utils::loadTexture(path);
}

void ImagePane::render() const {
  bgfx::setState(BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_RGB |
      BGFX_STATE_WRITE_Z | BGFX_STATE_BLEND_ALPHA);
  bgfx::setVertexBuffer(0, vertexBuffer);
  bgfx::setIndexBuffer(indexBuffer);
  bgfx::setTexture(0, textureColor, texture);
  bgfx::submit(viewId, program);
}
} // namespace views
