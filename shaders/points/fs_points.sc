$input v_texcoord0, v_color0

#include "./bgfx_shader.sh"

void main() {
  const float radius = 1.0;
  float distance = sqrt(dot(v_texcoord0, v_texcoord0));
  if (distance > radius) {
    discard;
  }
  gl_FragColor = v_color0;
}

