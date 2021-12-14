#version 420
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadow_mat[6];

out vec4 frag_pos; // out from geometry shader

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = shadow_mat[face] * frag_pos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
