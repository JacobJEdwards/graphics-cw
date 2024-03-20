#version 410 core
in vec3 FragPos;
out vec4 color;

// basic bounding box
void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
    if (FragPos.x > 0.5) {
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
    if (FragPos.y > 0.5) {
        color = vec4(0.0, 0.0, 1.0, 1.0);
    }
    if (FragPos.z > 0.5) {
        color = vec4(1.0, 1.0, 0.0, 1.0);
    }
}
