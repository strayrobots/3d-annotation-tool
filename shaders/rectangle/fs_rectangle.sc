$input v_texcoord0

#include "./bgfx_shader.sh"

const float borderWidth = 0.05;

void main() {
  float maxValue = 1.0 - borderWidth;
  if (v_texcoord0.x < maxValue && v_texcoord0.x > borderWidth &&
      v_texcoord0.y < maxValue && v_texcoord0.y > borderWidth) {
    gl_FragColor = vec4(0.4, 1.0, 0.4, 1.0);
  } else {
    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
  }
}

