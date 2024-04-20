#version 410 core

layout (vertices = 4) out;

uniform mat4 projection;
uniform mat4 view;

in vec3 WorldPos[];
in vec2 TexCoords[];

out vec2 TexCoordsF[];
out vec4 FragPosLightSpace[];

void main()
{
    gl_Position = gl_in[gl_InvocationID].gl_Position;
    TexCoordsF[gl_InvocationID] = TexCoords[gl_InvocationID];
    FragPosLightSpace[gl_InvocationID] = projection * view * vec4(WorldPos[gl_InvocationID], 1.0);
    gl_TessLevelInner[0] = 1.0;
    gl_TessLevelInner[1] = 1.0;
    gl_TessLevelOuter[0] = 1.0;
    gl_TessLevelOuter[1] = 1.0;
    gl_TessLevelOuter[2] = 1.0;
    gl_TessLevelOuter[3] = 1.0;
}





