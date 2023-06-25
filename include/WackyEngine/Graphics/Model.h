#ifndef WACKYENGINE_GRAPHICS_MODEL_H_
#define WACKYENGINE_GRAPHICS_MODEL_H_

#include <cstdint>

#include <vulkan/vulkan.h>

#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Core/Buffer.h"
#include "WackyEngine/Graphics/Vertex.h"

namespace WackyEngine
{
    class Model
    {
    private:
        Buffer* m_VertexBuffer;
        Buffer* m_IndexBuffer;

        std::uint32_t m_VertexCount;
        std::uint32_t m_IndexCount;

    public:
        static Model* GetTestCube();
        static Model* GetTestQuad();

        Model(const std::vector<Vertex>& vertices, const std::vector<std::uint16_t> indices);
        ~Model();

        void Bind(VkCommandBuffer buffer) const noexcept;
        void Draw(VkCommandBuffer buffer) const noexcept;
    };
}

#endif