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
        std::uint32_t TextureIndex;

        Vertex(Vector3 position, Vector3 colour, Vector2 texCoords, std::uint32_t texIndex) : Position(position), Colour(colour), TextureCoordinates(texCoords), TextureIndex(texIndex) { }
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
            std::vector<VkVertexInputAttributeDescription> attributes(4);

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

            attributes[3].binding = 0;
            attributes[3].location = 3;
            attributes[3].format = VK_FORMAT_R32_SINT;
            attributes[3].offset = offsetof(Vertex, TextureIndex);

            return attributes;
        }
    };
}

#endif