#pragma once
#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include <glm/mat4x4.hpp>

using namespace Graphics::Data;
using namespace Graphics;

namespace Game
{

	class StaticMeshComponent :
		public PrimitiveComponent
	{
	protected:

		using Base = PrimitiveComponent;

	private:

      StaticMeshRenderData m_renderData;

	public:

		StaticMeshComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const StaticMeshRenderData& renderData);

		virtual ~StaticMeshComponent();

      virtual ComponentType GetComponentType() const override;

		virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const StaticMeshRenderData& GetRenderData() const {

         return m_renderData;
      }

      std::shared_ptr<IMaterial> GetMaterial() const;
	};

}

