#include "WackyEngine/Graphics/Model.h"

#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    Model* Model::GetTestCube()
    {
        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f, 0}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f, 0}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 4/5
            {{-0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},

            {{0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // 6/7

            {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 8/9
            {{0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},

            {{-0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 10/11
            {{0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}
        };

        const std::vector<std::uint16_t> indices = { 0, 1, 2, 2, 3, 0, // front
                                                     4, 0, 3, 3, 5, 4, // left
                                                     1, 6, 7, 7, 2, 1, // right
                                                     8, 9, 1, 1, 0, 8, // top
                                                     2, 11, 10, 10, 3, 2, // bottom
                                                     4, 5, 7, 7, 6, 4 }; // back

        return new Model(vertices, indices);
    }

    Model* Model::GetTestQuad()
    {
        // const std::vector<Vertex> vertices = {
        //     {{-0.5f, -0.5f, 0}, {1.0f, 0.0f, 0.3f}, {0.0f, 0.0f}},
        //     {{0.5f, -0.5f, 0}, {0.0f, 1.0f, 0.4f}, {1.0f, 0.0f}},
        //     {{0.5f, 0.5f, 0}, {0.2f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        //     {{-0.5f, 0.5f, 0}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        // };

        const std::vector<Vertex> vertices = {
            {{100, 200, 0}, {1.0f, 0.0f, 0.3f}, {0.0f, 0.0f}},
            {{400, 200, 0}, {0.0f, 1.0f, 0.4f}, {1.0f, 0.0f}},
            {{400, 600, 0}, {0.2f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{100, 600, 0}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        };

        const std::vector<std::uint16_t> indices = { 0, 1, 2, 2, 3, 0 };

        return new Model(vertices, indices);
    }

    Model::Model(const std::vector<Vertex>& vertices, const std::vector<std::uint16_t> indices) : m_VertexCount(vertices.size()), m_IndexCount(indices.size())
    {
        // Vertex Buffer

        VkDeviceSize bufferSize(sizeof(Vertex) * vertices.size());

        m_VertexBuffer = new Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        Buffer* stagingBuffer = new Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer->SetData((void*)vertices.data(), (size_t)bufferSize);
        m_VertexBuffer->CopyBuffer(*stagingBuffer, bufferSize);

        // Index Buffer

        bufferSize = sizeof(std::uint16_t) * indices.size();

        m_IndexBuffer = new Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        delete stagingBuffer;
        stagingBuffer = new Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        stagingBuffer->SetData((void*)indices.data(), bufferSize);
        m_IndexBuffer->CopyBuffer(*stagingBuffer, bufferSize);
        
        delete stagingBuffer;
    }

    Model::~Model()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;
    }

    void Model::Bind(VkCommandBuffer buffer) const noexcept
    {
        VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBufferObject() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(buffer, m_IndexBuffer->GetBufferObject(), 0, VK_INDEX_TYPE_UINT16);
    }

    void Model::Draw(VkCommandBuffer buffer) const noexcept
    {
        vkCmdDrawIndexed(buffer, m_IndexCount, 1, 0, 0, 0);
    }
}