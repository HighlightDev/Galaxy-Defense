#pragma once

#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/ResourceManagerCore/MaterialInstanceDataProviders/InstancedStaticMeshMaterialDataProvider.h"

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct InstancedMeshComponentData;

	class InstancedStaticMeshComponent
		: public PrimitiveComponent,
		  public InstancedStaticMeshMaterialDataProvider
	{
	protected:
		MeshRenderData m_renderData;

	public:
		InstancedStaticMeshComponent(const std::shared_ptr<InstancedMeshComponentData> &meshComponentData, const MeshRenderData renderData);

		~InstancedStaticMeshComponent() override;

		eComponentType GetComponentType() const override;

		void SetIsEnabled(const bool bEnabled) override;

		void SetIsVisible(bool isVisible) override;

		void UnpausableTick(const float deltaTime) override;

		void CollectDataForSerialization(SerializeDataContainer &dataContainer) override;

		std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		inline const MeshRenderData &GetRenderData() const
		{
			return m_renderData;
		}

		std::shared_ptr<IMaterial> GetMaterial() const;

		BoundingBox3D GetTransformedBoundingBox() const override;

		// override of InstancedStaticMeshMaterialDataProvider methods

		std::string GetBatchKey() const override;

		std::weak_ptr<::EngineCore::Scene> GetSceneWp() const override;

		int32_t GetInstanceObjectId() const override;

		int32_t GetInstanceProxyId() const override;
	};

}
