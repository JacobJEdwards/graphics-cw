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

const float density = 0.1;
const float grassHeight = 1.0;
const float grassWidth = 10;

void main() {
    for (int i = 0; i < 3; i++) {
        // Calculate the grass blade direction perpendicular to the triangle face
        vec3 normal = normalize(cross(gs_in[1].FragPos - gs_in[0].FragPos, gs_in[2].FragPos - gs_in[0].FragPos));

        // Calculate the position of the grass blade
        vec3 grassPos = gs_in[i].FragPos;
        vec3 grassDir = normal;
        vec3 grassOffset = vec3(0.0, 0.0, 0.0);
        grassOffset.x = sin(grassPos.x * 0.1) * 0.1;
        grassOffset.z = cos(grassPos.z * 0.1) * 0.1;
        grassPos += grassOffset;

        // Calculate the position of the grass blade in light space
        vec4 fragPosLightSpace = gs_in[i].FragPosLightSpace;
        vec3 fragPosLightSpace3 = fragPosLightSpace.xyz / fragPosLightSpace.w;
        vec3 lightDir = normalize(vec3(0.0, 0.0, 0.0) - fragPosLightSpace3);
        float d = dot(normal, lightDir);
        float bias = max(0.05 * (1.0 - d), 0.005);
        fragPosLightSpace3 = fragPosLightSpace3 + normal * bias;
        fragPosLightSpace = vec4(fragPosLightSpace3, 1.0);

        // Calculate the grass blade vertices
        vec3 quadVertices[4];
        quadVertices[0] = grassPos + vec3(-grassWidth, 0.0, 0.0);
        quadVertices[1] = grassPos + vec3(grassWidth, 0.0, 0.0);
        quadVertices[2] = grassPos + vec3(-grassWidth, grassHeight, 0.0);
        quadVertices[3] = grassPos + vec3(grassWidth, grassHeight, 0.0);


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