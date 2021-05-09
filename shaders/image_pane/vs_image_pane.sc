$input a_position
$output v_pos, v_texture_coord

#include <bgfx_shader.sh>

void main() {
  v_pos = a_position;
  v_texture_coord.xy = (a_position.xy + 1.0) * 0.5;

  gl_Position = vec4(a_position, 1.0);
}

