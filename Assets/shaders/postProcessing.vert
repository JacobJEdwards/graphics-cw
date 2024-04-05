#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform bool shake = true;
uniform float shakeAmount = 0.001;
uniform float shakeSpeed = 50.0;
uniform float time = 0.0;

void main(){
    TexCoords = aTexCoords;

    if (shake){
        vec2 shakeOffset = vec2(sin(time * shakeSpeed), cos(time * shakeSpeed)) * shakeAmount;
        TexCoords += shakeOffset;
    }

    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}
