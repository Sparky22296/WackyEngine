#include "WackyEngine/Graphics/Renderers/Renderer2D.h"

#include <stdexcept>

#include "WackyEngine/Graphics/DescriptorUtil.h"
#include "WackyEngine/Graphics/SwapChain.h"
#include "WackyEngine/Graphics/UniformBufferObject.h"
#include "WackyEngine/Core/Context.h"

namespace WackyEngine
{
    Renderer2D::Renderer2D(const RenderPass* renderPass)
    {
        InitialiseUniformBuffers();
        InitialiseSampler();
        InitialiseDescriptors();
        InitialisePipelineLayout();
        InitialisePipeline(renderPass);
        
        // Buffer Setup

        m_VertexBuffer = new VertexBuffer(MAX_VERTICES);
        m_IndexBuffer = new IndexBuffer(MAX_INDICES);
    }
    
    Renderer2D::~Renderer2D()
    {
        delete m_VertexBuffer;
        delete m_IndexBuffer;

        vkDestroySampler(Context::GetDevice()->GetLogicalDevice(), m_Sampler, nullptr);

        vkDestroyDescriptorSetLayout(Context::GetDevice()->GetLogicalDevice(), m_GlobalDescriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(Context::GetDevice()->GetLogicalDevice(), m_GlobalDescriptorPool, nullptr);

        for (std::size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            delete m_UniformBuffers[i];
        }

        vkDestroyPipelineLayout(Context::GetDevice()->GetLogicalDevice(), m_PipelineLayout, nullptr);

        delete m_Pipeline;
    }

    void Renderer2D::Begin()
    {
        m_VertexBuffer->Reset();
        m_IndexBuffer->Reset();
        m_TextureBuffer.clear();
    }

    void Renderer2D::End(VkCommandBuffer cmdBuffer, const std::uint32_t currentIndex)
    {
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipeline());
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_GlobalDescriptorSets[currentIndex], 0, nullptr);

        m_VertexBuffer->Flush();
        m_IndexBuffer->Flush();

        // TEXTURES

        VkDescriptorImageInfo textureInfo { };
        std::vector<VkDescriptorImageInfo> textureInfos(m_TextureBuffer.size(), textureInfo);

        for (std::size_t i = 0; i < m_TextureBuffer.size(); ++i)
        {
            textureInfos[i].sampler = VK_NULL_HANDLE;
            textureInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            textureInfos[i].imageView = m_TextureBuffer[i]->GetImageView();
        }

        DescriptorWriter(m_GlobalDescriptorSets[currentIndex])
            .WriteImage(2, m_TextureBuffer.size(), VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, textureInfos.data())
            .Write();

        VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBufferObject() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(cmdBuffer, m_IndexBuffer->GetBufferObject(), 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(cmdBuffer, m_IndexBuffer->GetCount(), 1, 0, 0, 0);
    }

    void Renderer2D::DrawRectangle(const Rectangle& rect, const Vector3& colour, Texture* texture)
    {
        m_TextureBuffer.push_back(texture);

        m_IndexBuffer->AddIndex(static_cast<std::uint16_t>(m_VertexBuffer->GetCount() + 0));
        m_IndexBuffer->AddIndex(static_cast<std::uint16_t>(m_VertexBuffer->GetCount() + 1));
        m_IndexBuffer->AddIndex(static_cast<std::uint16_t>(m_VertexBuffer->GetCount() + 2));
        m_IndexBuffer->AddIndex(static_cast<std::uint16_t>(m_VertexBuffer->GetCount() + 2));
        m_IndexBuffer->AddIndex(static_cast<std::uint16_t>(m_VertexBuffer->GetCount() + 3));
        m_IndexBuffer->AddIndex(static_cast<std::uint16_t>(m_VertexBuffer->GetCount() + 0));

        m_VertexBuffer->AddVertex(Vertex(Vector3(rect.X, rect.Y, 0.0f), colour, Vector2(0.0f, 0.0f), m_TextureBuffer.size() - 1));
        m_VertexBuffer->AddVertex(Vertex(Vector3(rect.GetRight(), rect.Y, 0.0f), colour, Vector2(1.0f, 0.0f), m_TextureBuffer.size() - 1));
        m_VertexBuffer->AddVertex(Vertex(Vector3(rect.GetRight(), rect.GetBottom(), 0.0f), colour, Vector2(1.0f, 1.0f), m_TextureBuffer.size() - 1));
        m_VertexBuffer->AddVertex(Vertex(Vector3(rect.X, rect.GetBottom(), 0.0f), colour, Vector2(0.0f, 1.0f), m_TextureBuffer.size() - 1));
    }

    void Renderer2D::InitialisePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo { };
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        
        VkPushConstantRange pushRangeInfo { };
        pushRangeInfo.size = sizeof(PC);
        pushRangeInfo.offset = 0;
        pushRangeInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        pipelineLayoutInfo.pSetLayouts = &m_GlobalDescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushRangeInfo;

        if (vkCreatePipelineLayout(Context::GetDevice()->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) 
        {
            throw std::runtime_error("Failed to create pipeline layout.");
        }
    }

    void Renderer2D::InitialisePipeline(const RenderPass* renderPass)
    {
        PipelineConfig config { };

        m_Pipeline->GetDefaultConfig(config);
        config.RenderPass = renderPass->GetRenderPass();
        config.PipelineLayout = m_PipelineLayout;
        m_Pipeline = new Pipeline(config);
    }

    void Renderer2D::InitialiseDescriptors()
    {
        // Descriptor Pool

        m_GlobalDescriptorPool = DescriptorPoolBuilder(SwapChain::MAX_FRAMES_IN_FLIGHT)
                                    .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                                    .AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                                    .AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, SwapChain::MAX_FRAMES_IN_FLIGHT * MAX_TEXTURES)
                                    .Build();

        // Descriptor Set Layout

        m_GlobalDescriptorSetLayout = DescriptorSetLayoutBuilder()
                                          // Binding 1: Uniform Buffer
                                          .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT)
                                          // Binding 2: Image Sampler
                                          .AddBinding(1, VK_DESCRIPTOR_TYPE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
                                          // Binding 3: Texture Array
                                          .AddBinding(2, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_TEXTURES, VK_SHADER_STAGE_FRAGMENT_BIT)
                                          .Build();

        // Descriptors

        m_GlobalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSetLayout> layouts(SwapChain::MAX_FRAMES_IN_FLIGHT, m_GlobalDescriptorSetLayout);

        VkDescriptorSetAllocateInfo descSetAllocInfo { };
        descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descSetAllocInfo.descriptorPool = m_GlobalDescriptorPool;
        descSetAllocInfo.descriptorSetCount = (std::uint32_t)SwapChain::MAX_FRAMES_IN_FLIGHT;
        descSetAllocInfo.pSetLayouts = layouts.data();

        if (vkAllocateDescriptorSets(Context::GetDevice()->GetLogicalDevice(), &descSetAllocInfo, m_GlobalDescriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create descriptor sets.");
        }

        // Writing Descriptors

        for (std::size_t i = 0; i < m_GlobalDescriptorSets.size(); ++i)
        {
            VkDescriptorBufferInfo bufferInfo { };
            bufferInfo.buffer = m_UniformBuffers[i]->GetBufferObject();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UBO);

            VkDescriptorImageInfo samplerInfo { };
            samplerInfo.sampler = m_Sampler;

            VkDescriptorImageInfo defaultTextureInfo { };
            defaultTextureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            defaultTextureInfo.imageView = VK_NULL_HANDLE;
            defaultTextureInfo.sampler = VK_NULL_HANDLE;

            std::vector<VkDescriptorImageInfo> defaultTextureInfos(MAX_TEXTURES, defaultTextureInfo);

            DescriptorWriter(m_GlobalDescriptorSets[i])
                .WriteBuffer(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &bufferInfo)
                .WriteImage(1, 1, VK_DESCRIPTOR_TYPE_SAMPLER, &samplerInfo)
                .WriteImage(2, MAX_TEXTURES, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, defaultTextureInfos.data())
                .Write();
        }
    }
    
    void Renderer2D::InitialiseSampler()
    {
        VkSamplerCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties { };
        vkGetPhysicalDeviceProperties(Context::GetDevice()->GetPhysicalDevice(), &properties);

        info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.unnormalizedCoordinates = VK_FALSE;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.mipLodBias = 0.0f;
        info.minLod = 0.0f;
        info.maxLod = 0.0f;

        if (vkCreateSampler(Context::GetDevice()->GetLogicalDevice(), &info, nullptr, &m_Sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture sampler.");
        }
    }

    void Renderer2D::InitialiseUniformBuffers()
    {
        m_UniformBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        for (std::size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_UniformBuffers[i] = new Buffer(sizeof(UBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
    }

    void Renderer2D::SetResolution(const std::uint32_t width, const std::uint32_t height)
    {
        UBO ubo { Matrix4::CreateOrthographic(width, height) };

        for (std::size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; ++i)
        {
            m_UniformBuffers[i]->SetData(&ubo, sizeof(UBO));
        }
    }
}