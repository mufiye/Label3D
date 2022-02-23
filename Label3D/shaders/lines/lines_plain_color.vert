#version 330 core

layout (location = 0) in vec3 vtx_position;	// point position
layout (location = 1) in vec3 vtx_color;	// point color

uniform mat4    MVP;
uniform vec4    default_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
uniform bool    per_vertex_color = false;

out vec4 vOutColor;

void main()
{
    gl_Position = MVP * vec4(vtx_position, 1.0);

    if (per_vertex_color)
    vOutColor = vec4(vtx_color, 1.0);
    else
        vOutColor = default_color;
}
