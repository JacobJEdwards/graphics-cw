#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

out VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);

    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = aNormal;
    vs_out.FragPos = vec3(vec4(aPos, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}
