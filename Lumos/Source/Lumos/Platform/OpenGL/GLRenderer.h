#pragma once

#include "Graphics/RHI/Pipeline.h"
#include "Maths/Maths.h"
#include "Core/OS/Window.h"
#include "Graphics/RHI/Renderer.h"
#include "GLSwapChain.h"
#include "Graphics/RHI/Definitions.h"
namespace Lumos
{
    class Window;
    namespace Graphics
    {
        class GLContext;
        class GLIndexBuffer;
        class GLVertexBuffer;
        class CommandBuffer;
        class Shader;

        class LUMOS_EXPORT GLRenderer : public Renderer
        {
        public:
            friend class Window;
            GLRenderer();
            ~GLRenderer();

            static GLRenderer* Instance()
            {
                return static_cast<GLRenderer*>(s_Instance);
            }

            void Begin() override;
            void InitInternal() override;

            void BindDescriptorSetsInternal(Graphics::Pipeline* pipeline, Graphics::CommandBuffer* commandBuffer, uint32_t dynamicOffset, Graphics::DescriptorSet** descriptorSets, uint32_t descriptorCount) override;
            void DrawInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, DataType dataType, void* indices) const override;
            void DrawIndexedInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start) const override;
            void SetRenderModeInternal(RenderMode mode);
            void OnResize(uint32_t width, uint32_t height) override;
            void PresentInternal() override;
            void PresentInternal(Graphics::CommandBuffer* commandBuffer) override;
            void SetDepthTestingInternal(bool enabled);
            void SetBlendInternal(bool enabled);
            void SetStencilTestInternal(bool enabled);
            void SetCullingInternal(bool enabled, bool front);
            void SetDepthMaskInternal(bool enabled);
            void SetViewportInternal(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
            void SetPixelPackType(PixelPackType type);

            void SetColourMaskInternal(bool r, bool g, bool b, bool a);

            void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination);
            void SetBlendEquationInternal(RendererBlendFunction blendEquation);
            void SetStencilFunctionInternal(StencilType type, uint32_t ref, uint32_t mask);
            void SetStencilOpInternal(StencilType fail, StencilType zfail, StencilType zpass);

            static void ClearInternal(uint32_t buffer);
            void ClearRenderTarget(Graphics::Texture* texture, Graphics::CommandBuffer* commandBuffer, glm::vec4 clearColour) override;

            const std::string& GetTitleInternal() const override;

            static void MakeDefault();

            int32_t& GetBoundVertexBuffer() { return m_BoundVertexBuffer; }
            int32_t& GetBoundIndexBuffer() { return m_BoundIndexBuffer; }

        protected:
            static Renderer* CreateFuncGL();
            std::string m_RendererTitle;
            int32_t m_BoundVertexBuffer = -1;
            int32_t m_BoundIndexBuffer  = -1;

            GLVertexBuffer* m_DefaultVertexBuffer;
            GLIndexBuffer* m_DefaultIndexBuffer;
        };
    }
}
