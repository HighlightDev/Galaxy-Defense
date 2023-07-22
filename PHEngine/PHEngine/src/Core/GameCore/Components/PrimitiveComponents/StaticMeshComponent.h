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

		StaticMeshRenderData m_renderData;

	public:
		StaticMeshComponent(const std::shared_ptr<MeshComponentData> &meshComponentData, const StaticMeshRenderData renderData);

		~StaticMeshComponent() override;

		eComponentType GetComponentType() const override;

		void SetIsEnabled(const bool bEnabled) override;

		void SetIsVisible(bool isVisible) override;

		void Tick(const float deltaTime) override;

		void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

		std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		void PostLevelInit() override;

		inline const StaticMeshRenderData &GetRenderData() const
		{
			return m_renderData;
		}

		std::shared_ptr<IMaterial> GetMaterial() const;
	};

}
