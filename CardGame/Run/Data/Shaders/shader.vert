#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform CameraUniformBufferObject {
    mat4 view;
    mat4 proj;
} cubo;

layout(binding = 1) uniform ModelUniformBufferObject {
    mat4 model;
} mubo;

void main() {
    gl_Position = cubo.proj * cubo.view * mubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}