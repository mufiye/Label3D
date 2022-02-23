#version 330 core
// please send comments or report bug to: liangliang.nan@gmail.com

layout (location = 0) in vec3 vtx_position;
layout (location = 1) in vec2 vtx_texcoord;
layout (location = 2) in int vtx_texId;
layout (location = 3) in vec3 vtx_normal;

uniform mat4 MVP;

out Data{
	int texId;
    vec2 texcoord;
    vec3 normal;
    vec3 position;
} DataOut;

void main() {
	
	DataOut.texId = vtx_texId;
    DataOut.texcoord = vtx_texcoord;
    DataOut.normal = vtx_normal;
    DataOut.position = vtx_position;

    gl_Position = MVP * vec4(vtx_position, 1.0);
}
