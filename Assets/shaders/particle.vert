#version 410 core

layout(location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 offset;// Offset to be passed

void main() {
    // Construct a rotation matrix that faces the camera
    mat4 viewRotation = mat4(
    view[0][0], view[1][0], view[2][0], 0,
    view[0][1], view[1][1], view[2][1], 0,
    view[0][2], view[1][2], view[2][2], 0,
    0, 0, 0, 1
    );

    // Combine the rotation with the model matrix
    // Transform the position
    vec4 transformedPosition = projection * viewRotation * vec4(position + offset, 1.0);

    // Set the output position
    gl_Position = transformedPosition;
}
