#version 410 core
layout(triangles, equal_spacing, cw) in;

in TC_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    mat3 TBN;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} te_in[];

out TE_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    mat3 TBN;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} te_out;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    vec3 pos = vec3(0.0);
    vec3 normal = vec3(0.0);
    vec3 tangent = vec3(0.0);
    vec3 bitangent = vec3(0.0);
    mat3 tbn = mat3(0.0);

    for (int i = 0; i < 3; i++) {
        pos += te_in[i].FragPos * gl_TessCoord[i];
        normal += te_in[i].Normal * gl_TessCoord[i];
        tangent += te_in[i].Tangent * gl_TessCoord[i];
        bitangent += te_in[i].Bitangent * gl_TessCoord[i];
        tbn += te_in[i].TBN * gl_TessCoord[i];
    }

    te_out.TexCoords = mix(mix(te_in[0].TexCoords, te_in[1].TexCoords, v), te_in[2].TexCoords, u);
    te_out.FragPos = pos;
    te_out.Normal = normalize(normal);
    te_out.Tangent = normalize(tangent);
    te_out.Bitangent = normalize(bitangent);
    te_out.TBN = normalize(tbn);
    te_out.FragPosLightSpace = mix(mix(te_in[0].FragPosLightSpace, te_in[1].FragPosLightSpace, v), te_in[2].FragPosLightSpace, u);

    gl_Position = gl_in[0].gl_Position * w + gl_in[1].gl_Position * u + gl_in[2].gl_Position * v;
}