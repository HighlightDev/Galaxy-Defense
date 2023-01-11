#pragma once
#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct MeshComponentData;

	class RuntimeGeneratedLineComponent : public StaticMeshComponent
	{
		glm::vec3 mLineBeginWorldSpacePosition;

		glm::vec3 mLineEndWorldSpacePosition;

	protected:
		using Base = StaticMeshComponent;

	public:
		RuntimeGeneratedLineComponent(const MeshComponentData &meshComponentData, const StaticMeshRenderData &renderData);

		std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		void Tick(const float deltaTime) override;

		void SetLineBeginWorldSpacePosition(const glm::vec3& position);

		void SetLineEndWorldSpacePosition(const glm::vec3& position);

		glm::vec3 GetLineBeginWorldSpacePosition() const;

		glm::vec3 GetLineEndWorldSpacePosition() const;
	};

}
