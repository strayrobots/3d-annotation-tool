
$input a_position, a_normal
$output v_pos, v_normal

//#include <bgfx_shader.sh>

void main() {
	v_pos = a_position.xyz;
	v_normal = a_normal; // mul(u_modelView, vec4(a_normal, 0.0) ).xyz;
	gl_Position.xyz = a_position; // mul(u_modelViewProj, vec4(pos, 1.0));
  gl_Position.w = 1.0;
}
