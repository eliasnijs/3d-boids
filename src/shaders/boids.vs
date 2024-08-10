#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_vel;

uniform float u_point_size;
uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 projection_transform;

out vec3 v_vel;

void main() {
  gl_Position = projection_transform * view_transform * model_transform * vec4(a_pos, 1.0);
	v_vel = a_vel;
}
