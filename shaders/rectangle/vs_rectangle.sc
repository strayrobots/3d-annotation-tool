$input a_position, a_texcoord0
$output v_texcoord0

#include "./bgfx_shader.sh"

uniform mat4 u_rotation;

void main() {
  vec4 position = mul(u_modelViewProj, vec4(a_position, 1.0));
  gl_Position = position;
  v_texcoord0 = a_texcoord0;
}

