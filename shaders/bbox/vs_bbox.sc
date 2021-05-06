$input a_position
$output v_pos

#include <bgfx_shader.sh>

uniform vec3 u_scale;

void main() {
  v_pos = a_position;
  gl_Position = mul(u_modelViewProj, vec4(u_scale * a_position, 1.0));
}

