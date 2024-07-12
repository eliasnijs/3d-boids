#version 330 core

in  vec4 vertex_color;
in  vec2 tex_coord;
out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;

void
main()
{
    color = mix(texture(tex1, tex_coord), texture(tex2, tex_coord), 0.6);
}

