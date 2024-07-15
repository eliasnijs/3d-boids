#version 330 core

in  vec4 vertex_color;
in  vec2 tex_coord;
out vec4 color;

uniform sampler2D tex1;

void
main()
{
    color = texture(tex1, tex_coord);
}

