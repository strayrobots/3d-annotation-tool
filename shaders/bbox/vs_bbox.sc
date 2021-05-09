$input a_position
$output v_pos

#include <bgfx_shader.sh>

uniform vec4 u_scale;

void main() {
  v_pos = a_position;
  gl_Position = mul(u_modelViewProj, vec4(u_scale.x * a_position.x, u_scale.y * a_position.y, u_scale.z * a_position.z, 1.0));
}

