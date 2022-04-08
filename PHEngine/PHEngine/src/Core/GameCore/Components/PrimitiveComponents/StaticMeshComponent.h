#pragma once
#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include <glm/mat4x4.hpp>

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct MeshComponentData;

	class StaticMeshComponent : public PrimitiveComponent
	{
	protected:
		using Base = PrimitiveComponent;

	private:
		StaticMeshRenderData m_renderData;

	public:
		StaticMeshComponent(const MeshComponentData &meshComponentData, const StaticMeshRenderData &renderData);

		virtual ~StaticMeshComponent();

		virtual ComponentType GetComponentType() const override;

		virtual void SetIsEnabled(const bool bEnabled) override;

		virtual void SetIsVisible(bool isVisible) override;

		virtual void Tick(const float deltaTime) override;

		virtual void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

		virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		virtual void PostLevelInit() override;

		inline const StaticMeshRenderData &GetRenderData() const
		{

			return m_renderData;
		}

		std::shared_ptr<IMaterial> GetMaterial() const;
	};

}
