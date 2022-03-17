$input a_position, a_color0
$output v_color0

#include <bgfx_shader.sh>
uniform vec4 u_active_point;

void main() {
  gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
  float distance = length(u_active_point.xyz - a_position.xyz);
  if (distance < 0.005) {
    v_color0 = vec4(0.21, 0.87, 0.39, 1.0);
  } else {
    v_color0 = a_color0;
  }
}

