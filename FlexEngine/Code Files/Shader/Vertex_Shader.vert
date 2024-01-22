#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout( push_constant ) uniform testcolor 
{
    mat4 model;
    vec4 color;
} pushtestcolor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoordinates;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoordinates;


void main() {
    gl_Position = ubo.proj * ubo.view * pushtestcolor.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoordinates = inTexCoordinates;
}