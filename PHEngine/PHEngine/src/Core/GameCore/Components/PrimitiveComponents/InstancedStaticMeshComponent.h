#pragma once

#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct InstancedMeshComponentData;

	class InstancedStaticMeshComponent : public PrimitiveComponent
	{
	protected:
		using Base = PrimitiveComponent;

		MeshRenderData m_renderData;

	public:
		InstancedStaticMeshComponent(const std::shared_ptr<InstancedMeshComponentData> &meshComponentData, const MeshRenderData renderData);

		~InstancedStaticMeshComponent() override;

		eComponentType GetComponentType() const override;

		void SetIsEnabled(const bool bEnabled) override;

		void SetIsVisible(bool isVisible) override;

		void Tick(const float deltaTime) override;

		void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

		std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		inline const MeshRenderData &GetRenderData() const
		{
			return m_renderData;
		}

		std::shared_ptr<IMaterial> GetMaterial() const;
	};

}
