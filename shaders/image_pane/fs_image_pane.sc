$input v_pos, v_texture_coord

#include <bgfx_shader.sh>

SAMPLER2D(textureColor, 0);

void main() {
  gl_FragColor = texture2D(textureColor, v_texture_coord);
}

