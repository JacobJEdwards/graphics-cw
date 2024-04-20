#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} gs_in[];// pass to fragment shader

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform mat4 projection;
uniform mat4 view;

void main() {
    for (int i = 0; i < 3; i++) {
        // Calculate the grass blade direction perpendicular to the triangle face
        vec3 normal = normalize(cross(gs_in[1].FragPos - gs_in[0].FragPos, gs_in[2].FragPos - gs_in[0].FragPos));

        // Grass blade length and width (you can adjust these)
        float bladeLength = 1.0;
        float bladeWidth = 0.1;

        // Grass blade positions
        vec3 p0 = gs_in[i].FragPos;
        vec3 p1 = p0 + normal * bladeLength;

        // Grass blade width direction
        vec3 right = cross(normal, vec3(0.0, 1.0, 0.0));

        // Calculate quad vertices
        vec3 quadVertices[4];
        quadVertices[0] = p0 - right * bladeWidth;
        quadVertices[1] = p0 + right * bladeWidth;
        quadVertices[2] = p1 - right * bladeWidth;
        quadVertices[3] = p1 + right * bladeWidth;

        // Emit the grass blade as two triangles
        for (int j = 0; j < 4; j++) {
            gl_Position = projection * view * vec4(quadVertices[j], 1.0);
            fs_in.TexCoords = gs_in[i].TexCoords;
            fs_in.FragPos = quadVertices[j];
            fs_in.Normal = normal;
            fs_in.FragPosLightSpace = gs_in[i].FragPosLightSpace;
            EmitVertex();
        }
        EndPrimitive();
    }
}
