/*
// tessellation control shader
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
*/


// tessellatione evaluation shader
#version 410 core

layout (triangles, equal_spacing, ccw) in;

in vec2 TexCoordsF[];
in vec4 FragPosLightSpace[];

out vec2 TexCoords[];
out vec4 FragPosLightSpaceF;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    vec3 pos = mix(gl_in[0].gl_Position.xyz, mix(gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz, v), u);
    pos = mix(pos, gl_in[3].gl_Position.xyz, w);


    TexCoords[0] = TexCoordsF[0];
    TexCoords[1] = TexCoordsF[1];
    TexCoords[2] = TexCoordsF[2];
    TexCoords[3] = TexCoordsF[3];

    FragPosLightSpaceF = FragPosLightSpace[0] * (1.0 - u - v - w) + FragPosLightSpace[1] * u + FragPosLightSpace[2] * v + FragPosLightSpace[3] * w;
    gl_Position = FragPosLightSpaceF;

    //gl_Position = projection * view * vec4(pos, 1.0);
    
}




