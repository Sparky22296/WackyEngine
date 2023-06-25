#ifndef WACKYENGINE_GRAPHICS_PIPELINE_H_
#define WACKYENGINE_GRAPHICS_PIPELINE_H_

#include "WackyEngine/Core/Device.h"
#include "WackyEngine/Graphics/RenderPass.h"
#include "WackyEngine/Graphics/Texture.h"

namespace WackyEngine
{
    struct PipelineConfig
    {
        PipelineConfig() = default;
        PipelineConfig(const PipelineConfig&) = delete;
        PipelineConfig& operator=(const PipelineConfig&) = delete;

        VkPipelineViewportStateCreateInfo ViewportInfo;
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo RasterisationInfo;
        VkPipelineMultisampleStateCreateInfo MultisampleInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
        std::vector<VkDynamicState> DynamicStateEnables;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo;
        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        uint32_t Subpass = 0;
    };

    class Pipeline
    {
    private:
        VkPipeline m_Pipeline;

        VkShaderModule CreateShaderModule(const std::string& shaderFile);

    public:
        Pipeline(const PipelineConfig& config);
        ~Pipeline();

        void GetDefaultConfig(PipelineConfig& config) const noexcept;

        inline VkPipeline GetPipeline() const noexcept { return m_Pipeline; }
    };
}

#endif