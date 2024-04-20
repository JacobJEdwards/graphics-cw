#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;// Only need 4 vertices for a quad

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} gs_in[];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

uniform mat4 projection;
uniform mat4 view;
uniform float time = 0.0;

const float bladeLength = 1.0;
const float bladeWidth = 1.0;

const float windFrequency = 2.0;
const float windStrength = 2.0;

void main() {
    vec3 normal = normalize(cross(gs_in[1].FragPos - gs_in[0].FragPos, gs_in[2].FragPos - gs_in[0].FragPos));

    for (int i = 0; i < 3; i++) {
        vec3 p0 = gs_in[i].FragPos;
        vec3 p1 = p0 + normal * bladeLength;

        vec3 right = cross(normal, vec3(0.0, 1.0, 0.0)) * bladeWidth;

        float wind = sin(time * windFrequency) * windStrength;
        p0 += right * wind;
        p1 += right * wind;

        // sway in the wind
        vec3 quadVertices[4];
        quadVertices[0] = p0 - right;
        quadVertices[1] = p0 + right;
        quadVertices[2] = p1 - right;
        quadVertices[3] = p1 + right;

        for (int j = 0; j < 4; j++) {
            gl_Position = projection * view * vec4(quadVertices[j], 1.0);
            fs_in.FragPos = quadVertices[j];
            fs_in.Normal = normal;
            EmitVertex();
        }
        EndPrimitive();
    }
}
