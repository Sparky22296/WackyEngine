#include "WackyEngine/Graphics/Pipeline.h"

#include <fstream>
#include <iostream>

#include "WackyEngine/Core/Context.h"
#include "WackyEngine/Graphics/Vertex.h"
#include "WackyEngine/Graphics/UniformBufferObject.h"

namespace WackyEngine
{
    Pipeline::Pipeline(const PipelineConfig& config)
    {
        // Initialising Shader Modules
        VkShaderModule vertexShaderModule = CreateShaderModule("shaders/shader.vert.spv");
        VkShaderModule fragmentShaderModule = CreateShaderModule("shaders/shader.frag.spv");

        VkPipelineShaderStageCreateInfo vertShaderStageInfo { };
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertexShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo { };
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragmentShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        // Initialising Fixed-Function State

        auto bindingDescriptions = Vertex::GetBindingDescriptions();
        auto attributeDescriptions = Vertex::GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<std::uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        
        // Creating Pipeline

        VkGraphicsPipelineCreateInfo pipelineInfo { };
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &config.InputAssemblyInfo;
        pipelineInfo.pViewportState = &config.ViewportInfo;
        pipelineInfo.pRasterizationState = &config.RasterisationInfo;
        pipelineInfo.pMultisampleState = &config.MultisampleInfo;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &config.ColorBlendInfo;
        pipelineInfo.pDynamicState = &config.DynamicStateInfo;

        pipelineInfo.layout = config.PipelineLayout;
        pipelineInfo.renderPass = config.RenderPass;
        pipelineInfo.subpass = config.Subpass;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(Context::GetDevice()->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline.");
        }

        vkDestroyShaderModule(Context::GetDevice()->GetLogicalDevice(), vertexShaderModule, nullptr);
        vkDestroyShaderModule(Context::GetDevice()->GetLogicalDevice(), fragmentShaderModule, nullptr);
    }

    Pipeline::~Pipeline()
    {
        vkDestroyPipeline(Context::GetDevice()->GetLogicalDevice(), m_Pipeline, nullptr);
    }

    VkShaderModule Pipeline::CreateShaderModule(const std::string& shaderFile)
    {
        std::ifstream file(shaderFile, std::ios::ate | std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Failed to open shader file");
        }

        std::size_t fileSize = file.tellg();
        char* buffer = new char[fileSize];
        file.seekg(0);
        file.read(buffer, fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = fileSize;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer);

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(Context::GetDevice()->GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module.");
        }

        delete[] buffer;

        return shaderModule;
    }

    void Pipeline::GetDefaultConfig(PipelineConfig& config) const noexcept
    {
        config.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        config.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config.ViewportInfo.viewportCount = 1;
        config.ViewportInfo.pViewports = nullptr;
        config.ViewportInfo.scissorCount = 1;
        config.ViewportInfo.pScissors = nullptr;

        config.RasterisationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config.RasterisationInfo.depthClampEnable = VK_FALSE;
        config.RasterisationInfo.rasterizerDiscardEnable = VK_FALSE;
        config.RasterisationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        config.RasterisationInfo.lineWidth = 1.0f;
        config.RasterisationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        config.RasterisationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        config.RasterisationInfo.depthBiasEnable = VK_FALSE;
        config.RasterisationInfo.depthBiasConstantFactor = 0.0f;
        config.RasterisationInfo.depthBiasClamp = 0.0f;
        config.RasterisationInfo.depthBiasSlopeFactor = 0.0f;

        config.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        config.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config.MultisampleInfo.minSampleShading = 1.0f;
        config.MultisampleInfo.pSampleMask = nullptr;
        config.MultisampleInfo.alphaToCoverageEnable = VK_FALSE;
        config.MultisampleInfo.alphaToOneEnable = VK_FALSE;

        config.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        config.ColorBlendAttachment.blendEnable = VK_FALSE;
        config.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        config.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        config.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        config.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        config.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        config.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        config.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config.ColorBlendInfo.logicOpEnable = VK_FALSE;
        config.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        config.ColorBlendInfo.attachmentCount = 1;
        config.ColorBlendInfo.pAttachments = &config.ColorBlendAttachment;
        config.ColorBlendInfo.blendConstants[0] = 0.0f;
        config.ColorBlendInfo.blendConstants[1] = 0.0f;
        config.ColorBlendInfo.blendConstants[2] = 0.0f;
        config.ColorBlendInfo.blendConstants[3] = 0.0f;

        config.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        config.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        config.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.DynamicStateEnables.size());
        config.DynamicStateInfo.pDynamicStates = config.DynamicStateEnables.data();
        config.DynamicStateInfo.flags = 0;
    }
}