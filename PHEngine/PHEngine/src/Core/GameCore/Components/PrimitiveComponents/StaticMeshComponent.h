#pragma once
#include "PrimitiveComponent.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"

#include <glm/mat4x4.hpp>

using namespace Graphics::Data;

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

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

      inline const StaticMeshRenderData& GetRenderData() const {

         return m_renderData;
      }
	};

}

