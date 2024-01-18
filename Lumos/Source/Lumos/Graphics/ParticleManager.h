#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "Scene/Serialisation.h"
#include "Graphics/RHI/Definitions.h"
#include "Graphics/RHI/Texture.h"
#include "Core/OS/FileSystem.h"

namespace Lumos
{
    struct Particle
    {
        glm::vec3 Position, Velocity;
        glm::vec4 Colour;
        float Life;
        float Size;

        Particle()
            : Position(0.0f)
            , Velocity(0.0f)
            , Colour(1.0f)
            , Life(0.0f)
            , Size(0.1f)
        {
        }
    };

    class ParticleEmitter
    {
    public:
        ParticleEmitter();
        ParticleEmitter(uint32_t amount);

        enum BlendType : uint8_t
        {
            Additive = 0,
            Alpha    = 1,
            Off      = 2
        };

        enum AlignedType : uint8_t
        {
            Aligned2D = 0,
            Aligned3D = 1,
            None      = 2
        };

        void Update(float dt, glm::vec3 emitterPosition = glm::vec3(0.0f));
        template <typename Archive>
        void save(Archive& archive) const
        {
            archive(m_ParticleCount);
            archive(m_ParticleLife);
            archive(m_ParticleSize);
            archive(m_InitialVelocity);
            archive(m_InitialColour);
            archive(m_Spread);
            archive(m_VelocitySpread);
            archive(m_FadeIn);
            archive(m_FadeOut);
            archive(m_NextParticleTime);
            archive(m_ParticleRate);
            archive(m_NumLaunchParticles);
            archive(m_IsAnimated);
            archive(m_Gravity);
            archive(m_AnimatedTextureRows);

            std::string newPath = "";
            if(m_Texture)
            {
                FileSystem::Get().AbsolutePathToFileSystem(m_Texture->GetFilepath(), newPath);
            }
            archive(newPath);

			archive(m_LifeSpread);
			archive(m_SortParticles);
			archive(m_DepthWrite);
			archive(m_BlendType);
			archive(m_AlignedType);
        }

        template <typename Archive>
        void load(Archive& archive)
        {
            archive(m_ParticleCount);
            archive(m_ParticleLife);
            archive(m_ParticleSize);
            archive(m_InitialVelocity);
            archive(m_InitialColour);
            archive(m_Spread);
            archive(m_VelocitySpread);
            archive(m_FadeIn);
            archive(m_FadeOut);
            archive(m_NextParticleTime);
            archive(m_ParticleRate);
            archive(m_NumLaunchParticles);
            archive(m_IsAnimated);
            archive(m_Gravity);
            archive(m_AnimatedTextureRows);

            std::string textureFilePath;
            archive(textureFilePath);

            if(!textureFilePath.empty())
            {
                Graphics::TextureDesc desc;
                desc.minFilter = Graphics::TextureFilter::LINEAR;
                desc.magFilter = Graphics::TextureFilter::LINEAR;
                desc.wrap      = Graphics::TextureWrap::CLAMP;
                m_Texture      = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile("particle", textureFilePath, desc));
            }
            else
                m_Texture = nullptr;

			if(SceneSerialisationVersion > 23)
			{
				archive(m_LifeSpread);
				archive(m_SortParticles);
				archive(m_DepthWrite);
				archive(m_BlendType);
				archive(m_AlignedType);
			}

            Init();
        }

        void SetTextureFromFile(const std::string& path);

        Particle* GetParticles() { return m_Particles; }

        // Getter methods
        const SharedPtr<Graphics::Texture>& GetTexture() const { return m_Texture; }
        uint32_t GetParticleCount() const { return m_ParticleCount; }
        float GetParticleLife() const { return m_ParticleLife; }
        float GetParticleSize() const { return m_ParticleSize; }
        const glm::vec3& GetInitialVelocity() const { return m_InitialVelocity; }
        const glm::vec4& GetInitialColour() const { return m_InitialColour; }
        const glm::vec3& GetSpread() const { return m_Spread; }
        const glm::vec3& GetVelocitySpread() const { return m_VelocitySpread; }
        const glm::vec3& GetGravity() const { return m_Gravity; }
        float GetNextParticleTime() const { return m_NextParticleTime; }
        float GetParticleRate() const { return m_ParticleRate; }
        uint32_t GetNumLaunchParticles() const { return m_NumLaunchParticles; }
        bool GetIsAnimated() const { return m_IsAnimated; }
        uint32_t GetAnimatedTextureRows() const { return m_AnimatedTextureRows; }
        bool GetSortParticles() const { return m_SortParticles; }
        BlendType GetBlendType() const { return m_BlendType; }
        float GetFadeIn() const { return m_FadeIn; }
        float GetFadeOut() const { return m_FadeOut; }
        float GetLifeSpread() const { return m_LifeSpread; }
        AlignedType GetAlignedType() const { return m_AlignedType; }
        bool GetDepthWrite() const { return m_DepthWrite; }

        // Setter methods
        void SetTexture(const SharedPtr<Graphics::Texture>& texture) { m_Texture = texture; }
        void SetParticleCount(uint32_t particleCount);
        void SetParticleLife(const float& particleLife) { m_ParticleLife = particleLife; }
        void SetParticleSize(const float& particleSize) { m_ParticleSize = particleSize; }
        void SetInitialVelocity(const glm::vec3& initialVelocity) { m_InitialVelocity = initialVelocity; }
        void SetInitialColour(const glm::vec4& initialColour) { m_InitialColour = initialColour; }
        void SetSpread(const glm::vec3& spread) { m_Spread = spread; }
        void SetVelocitySpread(const glm::vec3& velocitySpread) { m_VelocitySpread = velocitySpread; }
        void SetGravity(const glm::vec3& gravity) { m_Gravity = gravity; }
        void SetNextParticleTime(const float& nextParticleTime) { m_NextParticleTime = nextParticleTime; }
        void SetParticleRate(const float& particleRate) { m_ParticleRate = particleRate; }
        void SetNumLaunchParticles(uint32_t numLaunchParticles) { m_NumLaunchParticles = numLaunchParticles; }
        void SetIsAnimated(bool isAnimated) { m_IsAnimated = isAnimated; }
        void SetAnimatedTextureRows(uint32_t animatedTextureRows) { m_AnimatedTextureRows = animatedTextureRows; }
        void SetSortParticles(bool sortParticles) { m_SortParticles = sortParticles; }
        void SetBlendType(const BlendType& blendType) { m_BlendType = blendType; }
        void SetFadeIn(const float& fadeIn) { m_FadeIn = fadeIn; }
        void SetFadeOut(const float& fadeOut) { m_FadeOut = fadeOut; }
        void SetLifeSpread(const float& spread) { m_LifeSpread = spread; }
        void SetAlignedType(const AlignedType& aligned) { m_AlignedType = aligned; }
        void SetDepthWrite(bool DepthWrite) { m_DepthWrite = DepthWrite; }

        static const std::array<glm::vec2, 4>& GetDefaultUVs();
        std::array<glm::vec2, 4> GetAnimatedUVs(float currentLife, int numRows);
        std::array<glm::vec4, 4> GetBlendedAnimatedUVs(float currentLife, int numRows, float& outBlendAmount);

    private:
        void Init();
        uint32_t FirstUnusedParticle();
        void RespawnParticle(Particle& particle, glm::vec3 emitterPosition = glm::vec3(0.0f));

        Particle* m_Particles = nullptr;

        SharedPtr<Graphics::Texture> m_Texture;
        uint32_t m_ParticleCount       = 1024;
        float m_ParticleLife           = 3.0f;
        float m_ParticleSize           = 0.1f;
        glm::vec3 m_InitialVelocity    = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec4 m_InitialColour      = glm::vec4(1.0f);
        glm::vec3 m_Spread             = glm::vec3(2.0f, 0.0f, 2.0f);
        glm::vec3 m_VelocitySpread     = glm::vec3(0.0f, 0.0f, 0.0f);
        float m_FadeIn                 = -1.0f;
        float m_FadeOut                = -1.0f;
        float m_NextParticleTime       = 0.0f;
        float m_ParticleRate           = 0.1f;
        uint32_t m_NumLaunchParticles  = 10;
        bool m_IsAnimated              = false;
        glm::vec3 m_Gravity            = glm::vec3(0.0f, 0.0f, 0.0f);
        uint32_t m_AnimatedTextureRows = 4;
        float m_LifeSpread             = 0.1f;
        bool m_SortParticles           = false;
        bool m_DepthWrite              = false;
        BlendType m_BlendType          = BlendType::Additive;
        AlignedType m_AlignedType      = AlignedType::Aligned3D;

        Arena* m_Arena = nullptr;
    };

}
