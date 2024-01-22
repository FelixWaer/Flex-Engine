#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoordinates;

layout(location = 0) out vec4 outColor;

layout( push_constant ) uniform testcolor 
{
    mat4 model;
    vec4 color;
} pushtestcolor;

void main() {
    outColor = texture(texSampler, fragTexCoordinates);
}