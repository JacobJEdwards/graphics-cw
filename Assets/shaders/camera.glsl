/*
 * https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
 */

layout(std140) uniform Camera {
    vec3 position;
    vec3 up;
    vec3 right;
    vec3 front;
} camera;