$input v_texcoord0

#include "./bgfx_shader.sh"

const float borderWidth = 0.05;

const vec4 innerColor = vec4(0.4, 1.0, 0.5, 1.0);
const vec4 outerColor = vec4(0.0, 0.8, 0.0, 1.0);
const vec4 circleColor = vec4(0.8, 1.0, 0.8, 1.0);

uniform vec4 u_scale;

void main() {
  vec2 scaledPos = v_texcoord0.xy * u_scale.xy;
  float minScale = min(u_scale.x, u_scale.y);
  float scaledBorderWidthX = borderWidth * u_scale.y / minScale;
  float scaledBorderWidthY = borderWidth * u_scale.x / minScale;
  float borderWidthX = -1.0 + scaledBorderWidthX;
  float borderWidthY = -1.0 + scaledBorderWidthY;
  float maxValueX = (1.0 - scaledBorderWidthX);
  float maxValueY = (1.0 - scaledBorderWidthY);
  float norm = length(scaledPos.xy / minScale);

  bool withCircle = u_scale.w > 0.0f;

  if (withCircle && norm < 0.25) {
    gl_FragColor = circleColor;
  } else if (v_texcoord0.x < maxValueX && v_texcoord0.x > borderWidthX &&
      v_texcoord0.y < maxValueY && v_texcoord0.y > borderWidthY) {
    gl_FragColor = innerColor;
  } else {
    gl_FragColor = outerColor;
  }
}

