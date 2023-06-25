#include "WackyEngine/Graphics/GraphicsBuffers.h"

#include <iostream>

namespace WackyEngine
{
    // VERTEX BUFFER

    VertexBuffer::VertexBuffer(const std::size_t count)
        : m_Buffer((VkDeviceSize)count * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
          m_StagingBuffer((VkDeviceSize)count * sizeof(Vertex), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
          m_VertexArray(new Vertex[count]),
          m_FrontHandle(m_VertexArray)
    {
    }

    VertexBuffer::~VertexBuffer()
    {
        delete[] m_VertexArray;
    }

    void VertexBuffer::AddVertex(const Vertex& vertex)
    {
        memcpy(m_FrontHandle, &vertex, sizeof(Vertex));
        m_FrontHandle++;
    }

    void VertexBuffer::Flush()
    {
        m_StagingBuffer.SetData(m_VertexArray, (m_FrontHandle - m_VertexArray) * sizeof(Vertex));
        m_Buffer.CopyBuffer(m_StagingBuffer, (m_FrontHandle - m_VertexArray) * sizeof(Vertex));
    }

    // INDEX BUFFER

    IndexBuffer::IndexBuffer(const std::size_t count)
        : m_Buffer((VkDeviceSize)count * sizeof(std::uint16_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
          m_StagingBuffer((VkDeviceSize)count * sizeof(std::uint16_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
          m_IndexArray(new std::uint16_t[count]),
          m_FrontHandle(m_IndexArray)
    {
    }

    IndexBuffer::~IndexBuffer()
    {
        delete[] m_IndexArray;
    }

    void IndexBuffer::AddIndex(const std::uint16_t index)
    {
        memcpy(m_FrontHandle, &index, sizeof(std::uint16_t));
        m_FrontHandle++;
    }

    void IndexBuffer::Flush()
    {
        m_StagingBuffer.SetData(m_IndexArray, (m_FrontHandle - m_IndexArray) * sizeof(std::uint16_t));
        m_Buffer.CopyBuffer(m_StagingBuffer, (m_FrontHandle - m_IndexArray) * sizeof(std::uint16_t));
    }
}