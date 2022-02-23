#version 330 core
// please send comments or report bug to: liangliang.nan@gmail.com

layout (location = 0) in vec3 vtx_position;
layout (location = 1) in vec3 vtx_texcoord;
layout (location = 2) in vec3 vtx_normal;

uniform mat4 MVP;

out Data{
    vec3 texcoord;
    vec3 normal;
    vec3 position;
} DataOut;

void main() {

    DataOut.texcoord = vtx_texcoord;
    DataOut.normal = vtx_normal;
    DataOut.position = vtx_position;

    gl_Position = MVP * vec4(vtx_position, 1.0);
}
