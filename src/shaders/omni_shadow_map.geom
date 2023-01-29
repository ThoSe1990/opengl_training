#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 light_matrices[6];

out vec4 frag_pos;

void main()
{   
    // 6 sides of a cube 
    for (int face = 0 ; face < 6 ; ++face)
    {
        gl_Layer = face;
        // 3 points of a triangle
        for (int i = 0 ; i < 3 ; i++) {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = light_matrices[face] * frag_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}