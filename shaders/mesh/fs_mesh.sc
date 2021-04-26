$input v_pos, v_normal

#include <bgfx_shader.sh>

uniform vec4 u_light_dir;
uniform vec4 u_color;

void main() {
  vec3 color = vec3(u_color.x, u_color.y, u_color.z);
  vec3 light_dir = u_light_dir.xyz;

  gl_FragColor.xyz = color * max(dot(v_normal, light_dir) * 0.5 + 0.5, 0.25);
  gl_FragColor.w = u_color.w;
}
