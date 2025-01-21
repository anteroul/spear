#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragPosition;

void main() {
    fragPosition = inPosition;
    gl_Position = vec4(inPosition, 1.0);
}
