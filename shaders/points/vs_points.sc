$input a_position, a_texcoord0, i_data0, i_data1
$output v_texcoord0, v_color0

#include "./bgfx_shader.sh"

uniform mat4 u_rotation;

void main() {
  vec4 vPos = mul(u_rotation, vec4(a_position, 1.0));
  vec4 position = mul(u_modelViewProj, vec4(vPos.xyz + i_data0, 1.0));
  gl_Position = position;

  v_texcoord0 = a_texcoord0;
  v_color0 = i_data1;
}

