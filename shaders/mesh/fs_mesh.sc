$input v_pos, v_normal, v_color0

#include <bgfx_shader.sh>

uniform vec4 u_light_dir;

void main() {
  vec3 color = vec3(v_color0.x, v_color0.y, v_color0.z);
  vec3 light_dir = u_light_dir.xyz;

  gl_FragColor.xyz = color * max(dot(v_normal, light_dir) * 0.5 + 0.5, 0.25);
  gl_FragColor.w = v_color0.w;
}
