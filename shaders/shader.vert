#version 450

layout (binding = 0) uniform UniformBufferObject
{
    mat4 proj;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColour;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in int inTexIndex;

layout (location = 0) out vec4 fragColour;
layout (location = 1) out vec2 fragTexCoord;
layout (location = 2) out int fragTexIndex;

void main()
{
    gl_Position = ubo.proj * vec4(inPosition, 1.0);
    fragColour = vec4(inColour, 1.0);
    fragTexCoord = inTexCoord;
    fragTexIndex = inTexIndex;
}