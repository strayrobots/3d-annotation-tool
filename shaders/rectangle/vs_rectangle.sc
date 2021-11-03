$input a_position
$output v_texcoord0

#include "./bgfx_shader.sh"

uniform vec4 u_scale;

void main() {
  vec4 position = vec4(a_position.x * u_scale.x, a_position.y * u_scale.y, a_position.z, 1.0);
  gl_Position = mul(u_modelViewProj, position);
  v_texcoord0 = vec2((a_position.x + 1.0) * 0.5, (a_position.y + 1.0) * 0.5);
}

