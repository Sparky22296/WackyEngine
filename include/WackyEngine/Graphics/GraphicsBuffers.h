#ifndef WACKYENGINE_GRAPHICS_GRAPHICSBUFFERS_H_
#define WACKYENGINE_GRAPHICS_GRAPHICSBUFFERS_H_

#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Core/Buffer.h"
#include "WackyEngine/Graphics/Vertex.h"

namespace WackyEngine
{
    class VertexBuffer
    {
    private:
        Buffer m_Buffer;
        Buffer m_StagingBuffer;
        Vertex* m_VertexArray;
        Vertex* m_FrontHandle;

    public:
        VertexBuffer(const std::size_t count);
        ~VertexBuffer();

        inline void Reset() { m_FrontHandle = m_VertexArray; }
        inline VkBuffer GetBufferObject() const noexcept { return m_Buffer.GetBufferObject(); }
        inline std::size_t GetCount() const noexcept { return static_cast<std::size_t>(m_FrontHandle - m_VertexArray); }
        void AddVertex(const Vertex& vertex);
        void Flush();
    };

    class IndexBuffer
    {
    private:
        Buffer m_Buffer;
        Buffer m_StagingBuffer;
        std::uint16_t* m_IndexArray;
        std::uint16_t* m_FrontHandle;

    public:
        IndexBuffer(const std::size_t count);
        ~IndexBuffer();

        inline void Reset() { m_FrontHandle = m_IndexArray; }
        inline VkBuffer GetBufferObject() const noexcept { return m_Buffer.GetBufferObject(); }
        inline std::size_t GetCount() const noexcept { return static_cast<std::size_t>(m_FrontHandle - m_IndexArray); }
        void AddIndex(const std::uint16_t index);
        void Flush();
    };
}

#endif