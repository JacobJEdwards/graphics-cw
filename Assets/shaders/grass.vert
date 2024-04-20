#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

out VS_OUT {
    vec3 Normal;
    vec3 FragPos;
} vs_out;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);

    vs_out.Normal = aNormal;
    vs_out.FragPos = vec3(vec4(aPos, 1.0));
}
