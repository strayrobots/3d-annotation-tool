$input v_pos, v_normal

/*
 * Copyright 2011-2021 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bgfx_shader.sh>

void main() {
  vec3 light_dir = vec3(0.0, 1.0, -1.0);
  vec3 color = vec3(1.0, 0.0, 0.0);

  gl_FragColor.xyz = color * max(dot(v_normal, light_dir), 0.0);
  gl_FragColor.w = 1.0;
}
