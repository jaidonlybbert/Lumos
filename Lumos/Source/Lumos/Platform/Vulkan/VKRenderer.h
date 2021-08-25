#pragma once

#include "VK.h"
#include "VKContext.h"
#include "VKSwapChain.h"
#include "Graphics/RHI/DescriptorSet.h"
#include "Graphics/RHI/RenderPass.h"
#include "VKUniformBuffer.h"
#include "VKDescriptorSet.h"
#include "Graphics/RHI/Renderer.h"

#define NUM_SEMAPHORES 10

namespace Lumos
{
    namespace Graphics
    {
        class CommandBuffer;

        class LUMOS_EXPORT VKRenderer : public Renderer
        {
        public:
            VKRenderer() {}
            ~VKRenderer();

            static VKRenderer* GetRenderer()
            {
                return static_cast<VKRenderer*>(s_Instance);
            }
            
            static VKContext* GetGraphicsContext()
            {
                return static_cast<VKContext*>(s_Instance->GetGraphicsContext());
            }
            
            static VKSwapChain* GetMainSwapChain()
            {
                return static_cast<VKSwapChain*>(Renderer::GetMainSwapChain());
            }

            void InitInternal() override;
            void Begin() override;
            void OnResize(uint32_t width, uint32_t height) override;

            void PresentInternal() override;
            void PresentInternal(CommandBuffer* commandBuffer) override;

            void ClearRenderTarget(Graphics::Texture* texture, Graphics::CommandBuffer* commandBuffer) override;
            void ClearSwapChainImage() const;

            const std::string& GetTitleInternal() const override;

            void BindDescriptorSetsInternal(Graphics::Pipeline* pipeline, Graphics::CommandBuffer* commandBuffer, uint32_t dynamicOffset, std::vector<Graphics::DescriptorSet*>& descriptorSets) override;
            void DrawIndexedInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start) const override;
            void DrawInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, DataType datayType, void* indices) const override;
            
            const VkDescriptorPool& GetDescriptorPool() const
            {
                return m_DescriptorPool;
            };

            static VKContext::DeletionQueue& GetDeletionQueue(int frameIndex)
            {
                LUMOS_ASSERT(frameIndex < 3, "Unsupported Frame Index");
                return s_DeletionQueue[frameIndex];
            }

            static VKContext::DeletionQueue& GetCurrentDeletionQueue()
            {
                return s_DeletionQueue[(s_Instance && Application::Get().GetWindow()) ? GetMainSwapChain()->GetCurrentBufferIndex() : 0];
            }

            static void MakeDefault();

        protected:
            static Renderer* CreateFuncVulkan();

        private:
            uint32_t m_CurrentSemaphoreIndex = 0;

            std::string m_RendererTitle;
        
            VkDescriptorPool m_DescriptorPool;
            VkDescriptorSet m_DescriptorSetPool[16];
            static VKContext::DeletionQueue s_DeletionQueue[3];
        };
    }
}
