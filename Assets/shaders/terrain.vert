#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);

    vs_out.TexCoords = aTexCoords;
    vs_out.Normal = aNormal;
    vs_out.FragPos = vec3(vec4(aPos, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.Tangent = aTangent;
    vs_out.Bitangent = aBitangent;
}
