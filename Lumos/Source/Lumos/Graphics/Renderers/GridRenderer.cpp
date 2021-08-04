#include "Precompiled.h"
#include "GridRenderer.h"
#include "Graphics/RHI/Shader.h"
#include "Graphics/RHI/Framebuffer.h"
#include "Graphics/RHI/Texture.h"
#include "Graphics/RHI/UniformBuffer.h"
#include "Graphics/RHI/Renderer.h"
#include "Graphics/RHI/CommandBuffer.h"
#include "Graphics/RHI/Swapchain.h"
#include "Graphics/RHI/RenderPass.h"
#include "Graphics/RHI/Pipeline.h"
#include "Graphics/RHI/GraphicsContext.h"
#include "Graphics/GBuffer.h"
#include "Graphics/Mesh.h"
#include "Graphics/MeshFactory.h"
#include "Scene/Scene.h"
#include "Core/Application.h"
#include "Graphics/Camera/Camera.h"
#include "Maths/Transform.h"
#include "Graphics/Renderers/RenderGraph.h"

#include "CompiledSPV/Headers/Gridvertspv.hpp"
#include "CompiledSPV/Headers/Gridfragspv.hpp"

#include <imgui/imgui.h>

namespace Lumos
{
    namespace Graphics
    {
        GridRenderer::GridRenderer(uint32_t width, uint32_t height)
        {
            m_Pipeline = nullptr;

            IRenderer::SetScreenBufferSize(width, height);
            GridRenderer::Init();

            m_GridRes = 1.4f;
            m_GridSize = 500.0f;
        }

        GridRenderer::~GridRenderer()
        {
            delete m_Quad;
        }

        void GridRenderer::RenderScene()
        {
            LUMOS_PROFILE_FUNCTION();
            m_CurrentBufferID = 0;
            if(!m_RenderTexture)
                m_CurrentBufferID = Renderer::GetSwapchain()->GetCurrentBufferIndex();

            Begin();

            SetSystemUniforms(m_Shader.get());

            m_Pipeline->Bind(Renderer::GetSwapchain()->GetCurrentCommandBuffer());

            m_CurrentDescriptorSets[0] = m_DescriptorSet[0].get();

            m_Quad->GetVertexBuffer()->Bind(Renderer::GetSwapchain()->GetCurrentCommandBuffer(), m_Pipeline.get());
            m_Quad->GetIndexBuffer()->Bind(Renderer::GetSwapchain()->GetCurrentCommandBuffer());

            Renderer::BindDescriptorSets(m_Pipeline.get(), Renderer::GetSwapchain()->GetCurrentCommandBuffer(), 0, m_CurrentDescriptorSets);
            Renderer::DrawIndexed(Renderer::GetSwapchain()->GetCurrentCommandBuffer(), DrawType::TRIANGLE, m_Quad->GetIndexBuffer()->GetCount());

            m_Quad->GetVertexBuffer()->Unbind();
            m_Quad->GetIndexBuffer()->Unbind();

            End();

            //if(!m_RenderTexture)
            //Renderer::Present((m_CommandBuffers[Renderer::GetSwapchain()->GetCurrentBufferIndex()].get()));
        }

        enum VSSystemUniformIndices : int32_t
        {
            VSSystemUniformIndex_InverseProjectionViewMatrix = 0,
            VSSystemUniformIndex_Size
        };

        void GridRenderer::Init()
        {
            LUMOS_PROFILE_FUNCTION();
            //m_Shader = Application::Get().GetShaderLibrary()->GetResource("//CoreShaders/Grid.shader");
            m_Shader = Graphics::Shader::CreateFromEmbeddedArray(spirv_Gridvertspv.data(), spirv_Gridvertspv_size, spirv_Gridfragspv.data(), spirv_Gridfragspv_size);

            m_Quad = Graphics::CreatePlane(5000.0f, 5000.f, Maths::Vector3(0.0f, 1.0f, 0.0f));

            AttachmentInfo textureTypes[2] = {
                { TextureType::COLOUR, TextureFormat::RGBA8 },
                { TextureType::DEPTH, TextureFormat::DEPTH },
            };

            Graphics::RenderPassDesc renderpassCI;
            renderpassCI.attachmentCount = 2;
            renderpassCI.textureType = textureTypes;
            renderpassCI.clear = false;

            m_RenderPass = Graphics::RenderPass::Get(renderpassCI);

            Graphics::DescriptorDesc info {};
            info.layoutIndex = 0;
            info.shader = m_Shader.get();
            m_DescriptorSet.resize(1);
            m_DescriptorSet[0] = SharedRef<Graphics::DescriptorSet>(Graphics::DescriptorSet::Create(info));

            CreateGraphicsPipeline();
            UpdateUniformBuffer();
            CreateFramebuffers();

            m_CurrentDescriptorSets.resize(1);
        }

        void GridRenderer::Begin()
        {
            LUMOS_PROFILE_FUNCTION();
            m_RenderPass->BeginRenderpass(Renderer::GetSwapchain()->GetCurrentCommandBuffer(), Maths::Vector4(0.0f), m_Framebuffers[m_CurrentBufferID].get(), Graphics::INLINE, m_ScreenBufferWidth, m_ScreenBufferHeight);
        }

        void GridRenderer::BeginScene(Scene* scene, Camera* overrideCamera, Maths::Transform* overrideCameraTransform)
        {
            LUMOS_PROFILE_FUNCTION();
            auto& registry = scene->GetRegistry();

            if(overrideCamera)
            {
                m_Camera = overrideCamera;
                m_CameraTransform = overrideCameraTransform;
            }
            else
            {
                auto cameraView = registry.view<Camera>();
                if(!cameraView.empty())
                {
                    m_Camera = &cameraView.get<Camera>(cameraView.front());
                    m_CameraTransform = registry.try_get<Maths::Transform>(cameraView.front());
                }
            }

            if(!m_Camera || !m_CameraTransform)
                return;

            auto proj = m_Camera->GetProjectionMatrix();

            UniformBufferObjectFrag test;
            test.res = m_GridRes;
            test.scale = m_GridSize;
            test.cameraPos = m_CameraTransform->GetWorldPosition();
            test.maxDistance = m_MaxDistance;

            auto invViewProj = proj * m_CameraTransform->GetWorldMatrix().Inverse();
            m_DescriptorSet[0]->SetUniform("UniformBufferObject", "u_MVP", &invViewProj);
            m_DescriptorSet[0]->SetUniformBufferData("UniformBuffer", &test);
			m_DescriptorSet[0]->Update();
        }

        void GridRenderer::End()
        {
            LUMOS_PROFILE_FUNCTION();
            m_RenderPass->EndRenderpass(Renderer::GetSwapchain()->GetCurrentCommandBuffer());
        }

        void GridRenderer::SetSystemUniforms(Shader* shader) const
        {
            LUMOS_PROFILE_FUNCTION();
        }

        void GridRenderer::OnImGui()
        {
            LUMOS_PROFILE_FUNCTION();
            ImGui::TextUnformatted("Grid Renderer");

            if(ImGui::TreeNode("Parameters"))
            {
                ImGui::DragFloat("Resolution", &m_GridRes, 1.0f, 0.0f, 10.0f);
                ImGui::DragFloat("Scale", &m_GridSize, 1.0f, 1.0f, 10000.0f);
                ImGui::DragFloat("Max Distance", &m_MaxDistance, 1.0f, 1.0f, 10000.0f);

                ImGui::TreePop();
            }
        }

        void GridRenderer::OnResize(uint32_t width, uint32_t height)
        {
            LUMOS_PROFILE_FUNCTION();
            m_Framebuffers.clear();

            SetScreenBufferSize(width, height);

            UpdateUniformBuffer();
            CreateFramebuffers();
        }

        void GridRenderer::CreateGraphicsPipeline()
        {
            LUMOS_PROFILE_FUNCTION();

            Graphics::PipelineDesc pipelineCreateInfo;
            pipelineCreateInfo.shader = m_Shader;
            pipelineCreateInfo.renderpass = m_RenderPass;
            pipelineCreateInfo.polygonMode = Graphics::PolygonMode::FILL;
            pipelineCreateInfo.cullMode = Graphics::CullMode::NONE;
            pipelineCreateInfo.transparencyEnabled = true;

            m_Pipeline = Graphics::Pipeline::Get(pipelineCreateInfo);
        }

        void GridRenderer::UpdateUniformBuffer()
        {
            LUMOS_PROFILE_FUNCTION();
        }

        void GridRenderer::SetRenderTarget(Texture* texture, bool rebuildFramebuffer)
        {
            LUMOS_PROFILE_FUNCTION();
            m_RenderTexture = texture;

            if(!rebuildFramebuffer)
                return;

            m_Framebuffers.clear();

            CreateFramebuffers();
        }

        void GridRenderer::CreateFramebuffers()
        {
            LUMOS_PROFILE_FUNCTION();
            TextureType attachmentTypes[2];
            attachmentTypes[0] = TextureType::COLOUR;
            attachmentTypes[1] = TextureType::DEPTH;

            Texture* attachments[2];
            FramebufferDesc bufferInfo {};
            bufferInfo.width = m_ScreenBufferWidth;
            bufferInfo.height = m_ScreenBufferHeight;
            bufferInfo.attachmentCount = 2;
            bufferInfo.renderPass = m_RenderPass.get();
            bufferInfo.attachmentTypes = attachmentTypes;

            attachments[1] = Application::Get().GetRenderGraph()->GetGBuffer()->GetDepthTexture();

            if(m_RenderTexture)
            {
                attachments[0] = m_RenderTexture;
                bufferInfo.attachments = attachments;
                bufferInfo.screenFBO = false;
                m_Framebuffers.emplace_back(Framebuffer::Get(bufferInfo));
            }
            else
            {
                for(uint32_t i = 0; i < Renderer::GetSwapchain()->GetSwapchainBufferCount(); i++)
                {
                    bufferInfo.screenFBO = true;
                    attachments[0] = Renderer::GetSwapchain()->GetImage(i);
                    bufferInfo.attachments = attachments;

                    m_Framebuffers.emplace_back(Framebuffer::Get(bufferInfo));
                }
            }
        }
    }
}
