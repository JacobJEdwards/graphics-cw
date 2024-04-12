#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out float height;
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main()
{
    height = aPos.y;
    gl_Position = projection * view * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    Normal = aNormal;

    FragPos = vec3(vec4(aPos, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}
