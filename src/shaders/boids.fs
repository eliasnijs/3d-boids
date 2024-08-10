#version 300 es
precision mediump float;

in vec3 v_vel;

out vec4 out_color;

uniform sampler2D tex;

void main() {
    out_color = vec4(1.0, 1.0, 1.0, 1.0);
}
