#version 330 core
// please send comments or report bug to: liangliang.nan@gmail.com

out  vec4    outputF;

void main()
{
    // NOTE: For points, the gl_PrimitiveID represent the ID of the input points.
    //		 For polygonal models, the gl_PrimitiveID represent the ID of triangles (the GPU assembles
    //		 all primitive type, e.g., triangle fan, polygon, triangle strips as triangles).
    //		 If you're selecting a single point, the id can also be computed in vertex shader using gl_VertexID.
    int id = gl_PrimitiveID;
    outputF.r = ((id & 0x000000FF) >>  0) / 255.0;
    outputF.g = ((id & 0x0000FF00) >>  8) / 255.0;
    outputF.b = ((id & 0x00FF0000) >> 16) / 255.0;
    outputF.a = 1.0;
} 