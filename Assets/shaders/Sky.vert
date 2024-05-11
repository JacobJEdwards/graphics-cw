#version 410 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    vs_out.FragPos = aPos;
    // vs_out.FragPosWorld = vec3(view * projection * vec4(vs_out.FragPos, 1.0));
    vs_out.FragPosWorld = vs_out.FragPos;

    vs_out.Normal = mat3(transpose(inverse(model))) * aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position.z = gl_Position.w;

    vs_out.TexCoords = aPos.xy;

}

