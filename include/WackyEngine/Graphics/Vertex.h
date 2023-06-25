#ifndef WACKYENGINE_GRAPHICS_VERTEX_H_
#define WACKYENGINE_GRAPHICS_VERTEX_H_

#include <vector>

#include <vulkan/vulkan.h>

#include "WackyEngine/Math/Vector2.h"
#include "WackyEngine/Math/Vector3.h"

namespace WackyEngine
{
    struct Vertex
    {
        Vector3 Position;
        Vector3 Colour;
        Vector2 TextureCoordinates;

        Vertex(Vector3 position, Vector3 colour, Vector2 textCoords) : Position(position), Colour(colour), TextureCoordinates(textCoords) { }
        Vertex() { }

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions() 
        {
            std::vector<VkVertexInputBindingDescription> descriptions(1);

            descriptions[0].binding = 0;
            descriptions[0].stride = sizeof(Vertex);
            descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return descriptions;
        }

        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
        {
            std::vector<VkVertexInputAttributeDescription> attributes(3);

            attributes[0].binding = 0;
            attributes[0].location = 0;
            attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributes[0].offset = offsetof(Vertex, Position);
            
            attributes[1].binding = 0;
            attributes[1].location = 1;
            attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributes[1].offset = offsetof(Vertex, Colour);

            attributes[2].binding = 0;
            attributes[2].location = 2;
            attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributes[2].offset = offsetof(Vertex, TextureCoordinates);

            return attributes;
        }
    };
}

#endif