#version 450

layout (binding = 1) uniform sampler texSampler;
layout (binding = 2) uniform texture2D textures[8];

layout(location = 0) in vec4 fragColour;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in int fragTexIndex;

layout(location = 0) out vec4 outColour;

void main() 
{
    outColour = texture(sampler2D(textures[fragTexIndex], texSampler), fragTexCoord);
}