$input v_pos, v_normal

#include <bgfx_shader.sh>

void main() {
  vec3 light_dir = vec3(0.2, 1.0, -1.0);
  vec3 color = vec3(0.92, 0.59, 0.2);

  gl_FragColor.xyz = color * max(dot(v_normal, light_dir) * 0.5 + 0.5, 0.25);
  gl_FragColor.w = 1.0;
}
