#ifndef WACKYENGINE_GRAPHICS_RENDERERS_RENDERER2D_H_
#define WACKYENGINE_GRAPHICS_RENDERERS_RENDERER2D_H_

#include "WackyEngine/Graphics/Pipeline.h"
#include "WackyEngine/Graphics/Model.h"
#include "WackyEngine/Graphics/Vertex.h"
#include "WackyEngine/Math/Matrix4.h"
#include "WackyEngine/Math/Rectangle.h"
#include "WackyEngine/Graphics/GraphicsBuffers.h"

namespace WackyEngine
{
    class Renderer2D
    {
    private:
        struct UBO
        {
            Matrix4 ProjectionMatrix;
        };

        struct PC
        {
            std::uint32_t TextureIndex, test1, test2, test3;
        };

        const std::size_t MAX_TEXTURES = 8;
        const std::size_t MAX_QUADS = 10000;
        const std::size_t MAX_VERTICES = MAX_QUADS * 4;
        const std::size_t MAX_INDICES = MAX_QUADS * 6;

        Pipeline* m_Pipeline;
        VkPipelineLayout m_PipelineLayout;

        // Descriptors
        VkDescriptorPool m_GlobalDescriptorPool;
        VkDescriptorSetLayout m_GlobalDescriptorSetLayout;
        std::vector<VkDescriptorSet> m_GlobalDescriptorSets;

        // Buffers
        VertexBuffer* m_VertexBuffer;
        IndexBuffer* m_IndexBuffer;
        std::vector<Buffer*> m_UniformBuffers;
        std::vector<Texture*> m_TextureBuffer;
        VkSampler m_Sampler;

        void InitialisePipelineLayout();
        void InitialisePipeline(const RenderPass* renderPass);
        void InitialiseDescriptors();
        void InitialiseSampler();
        void InitialiseUniformBuffers();

    public:
        Renderer2D(const RenderPass* renderPass);
        ~Renderer2D();

        void Begin();
        void End(VkCommandBuffer cmdBuffer, const std::uint32_t currentIndex);

        void DrawRectangle(const Rectangle& rect, const Vector3& colour, Texture* texture);

        void SetResolution(const std::uint32_t width, const std::uint32_t height);
    };
}

#endif