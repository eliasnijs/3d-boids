#version 330 core

layout (location = 0) in  vec3  a_pos;
layout (location = 1) in  vec3  a_color;
layout (location = 2) in  vec2  a_tex_coord;

out vec4 vertex_color;
out vec2 tex_coord;

uniform mat4 transform;

void
main()
{
    gl_Position  = transform * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
    vertex_color = vec4(a_color, 1.0);
    tex_coord = vec2(a_tex_coord.x, a_tex_coord.y);
}

