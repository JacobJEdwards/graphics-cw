#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;// Only need 4 vertices for a quad

/*
 * https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
 */

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} gs_in[];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform mat4 projection;
uniform mat4 view;
uniform float time = 0.0;

uniform float bladeLength = 0.8;
uniform float bladeWidth = 0.2;

uniform float density = 0.3;
uniform float gradient = 0.3;

uniform float windFrequency = 2.0;
uniform float windStrength = 10.0;


void main() {
    vec3 normal = normalize(cross(gs_in[1].FragPos - gs_in[0].FragPos, gs_in[2].FragPos - gs_in[0].FragPos));
    float wind = sin(time * windFrequency) * windStrength;
    vec3 right = cross(normal, vec3(0.0, 1.0, 0.0)) * bladeWidth;

    for (int i = 0; i < 3; i++) {
        vec3 p0 = gs_in[i].FragPos;
        vec3 p1 = p0 + normal * bladeLength;

        wind += sin(p0.y * 10.0) * 0.1;

        p1 += right * wind;
        p0.x += sin(p0.y * 10.0) * 0.1;
        p1.x += sin(p1.y * 10.0) * 0.1;

        vec3 quadVertices[4];
        quadVertices[0] = p0 - right;
        quadVertices[1] = p0 + right;
        quadVertices[2] = p1 - right;
        quadVertices[3] = p1 + right;

        vec2 texCoords[4];
        texCoords[0] = vec2(0.0, 0.0);
        texCoords[1] = vec2(1.0, 0.0);
        texCoords[2] = vec2(0.0, 1.0);
        texCoords[3] = vec2(1.0, 1.0);

        for (int j = 0; j < 4; j++) {
            gl_Position = projection * view * vec4(quadVertices[j], 1.0);
            fs_in.FragPos = quadVertices[j];
            fs_in.Normal = normal;
            fs_in.TexCoords = texCoords[j];
            EmitVertex();
        }
        EndPrimitive();
    }
}

