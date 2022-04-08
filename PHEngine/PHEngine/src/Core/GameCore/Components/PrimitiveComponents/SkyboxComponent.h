#pragma once
#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/OpenGL/Shader/Uniform.h"
#include "Core/GraphicsCore/RenderData/SkyboxRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <memory>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct SkyboxComponentData;

	class SkyboxComponent : public PrimitiveComponent
	{
	private:
		float m_rotateSpeed;

		SkyboxRenderData m_renderData;

	protected:
		using Base = PrimitiveComponent;

	public:
		SkyboxComponent(const SkyboxComponentData &data, const SkyboxRenderData &renderData);

		virtual ~SkyboxComponent();

		virtual void SetIsEnabled(const bool bEnabled) override;

		virtual void SetIsVisible(bool isVisible) override;

		virtual void Tick(const float deltaTime) override;

		virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

		virtual ComponentType GetComponentType() const override;

		virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		void SetRotateSpeed(float rotateSpeed);

		float GetRotateSpeed() const;

		const SkyboxRenderData &GetRenderData() const;

		std::shared_ptr<IMaterial> GetMaterial() const;
	};

}
