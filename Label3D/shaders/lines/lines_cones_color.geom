#version 150
// please send comments or report bug to: liangliang.nan@gmail.com

// It uses geometry shader for vertex generation.
// The code does not cover round caps.

//#extension GL_EXT_geometry_shader4 : enable

layout(lines) in;
layout(triangle_strip, max_vertices = 5) out;

uniform mat4    PROJ;
uniform bool    perspective;
uniform float    radius;

in  vec4 vOutColor[];

out vec4 gOutColor;

out Data{
    vec3 point;// camera space
    vec3 axis;
    vec3 base;
    vec3 end;// the peak of the cone
    vec3 U;
    vec3 V;
} DataOut;


void main()
{
    vec3  base = gl_in[0].gl_Position.xyz;// in camera space
    vec3  top = gl_in[1].gl_Position.xyz;// in camera space
    vec3  len_dir = top - base;
    vec3  view_dir = vec3(0, 0, 1);// this is view direction for orthographic mode in camera space
    if (perspective)
    view_dir = normalize(vec3(0) - base);// camera pos is (0, 0, 0) in camera space

    vec3 left = cross(view_dir, len_dir);
    vec3 up = cross(left, len_dir);
    DataOut.axis = normalize(len_dir);
    DataOut.U = normalize(up);
    DataOut.V = normalize(left);
    left = radius * DataOut.V;
    up = radius * DataOut.U;

    DataOut.base = base;
    DataOut.end = top;

    float b = dot(view_dir, len_dir);
    if (b < 0) {
        // Vertex 1
        DataOut.point = top;
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[1];
        EmitVertex();

        // Vertex 2
        DataOut.point = (base + left - up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();

        // Vertex 3
        DataOut.point = (base - left - up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();

        // Vertex 4
        DataOut.point = (base + left + up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();

        // Vertex 5
        DataOut.point = (base - left + up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();
    }
    else {
        // Vertex 1
        DataOut.point = top;
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[1];
        EmitVertex();

        // Liangliang: determining the minimum extend for this case is tricky, so I just use
        //             an enlarge factor to make sure all the entire one is covered.
        float factor = 1.1;// It seems 1.1 is large enough

        // Vertex 2
        DataOut.point = (base - left * factor + up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();

        // Vertex 3
        DataOut.point = (base + left * factor + up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();

        // Vertex 4
        DataOut.point = (base - left - up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();

        // Vertex 5
        DataOut.point = (base + left - up);
        gl_Position = PROJ  * vec4(DataOut.point, 1.0);
        gOutColor = vOutColor[0];
        EmitVertex();
    }

    EndPrimitive();
}
