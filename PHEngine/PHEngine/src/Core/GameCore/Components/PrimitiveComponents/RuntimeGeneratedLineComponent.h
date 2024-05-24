#pragma once
#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/ResourceManagerCore/Pool/PoolParameters/RuntimeGeneratedMeshPoolParameters.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using namespace Resources;
using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct MeshComponentData;

	class RuntimeGeneratedLineComponent : public StaticMeshComponent
	{
		using Base = StaticMeshComponent;

		RuntimeGeneratedMeshPoolParameters mRtMeshParams;

		glm::vec3 mLineBeginWorldSpacePosition;

		glm::vec3 mLineEndWorldSpacePosition;

		float mLineWidth;

	protected:
		bool mIsRenderDataDirty{false};

	public:
		RuntimeGeneratedLineComponent(const std::shared_ptr<MeshComponentData> &meshComponentData,
									  const MeshRenderData &renderData,
									  const RuntimeGeneratedMeshPoolParameters &rtMeshParams);

		~RuntimeGeneratedLineComponent() override;

		std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		void UnpausableTick(const float deltaTime) override;

		const RuntimeGeneratedMeshPoolParameters &GetRuntimeMeshPoolParameters() const;

		void SetLineBeginWorldSpacePosition(const glm::vec3 &position);

		void SetLineEndWorldSpacePosition(const glm::vec3 &position);

		void SetLineWidth(const float lineWidth);

		float GetLineWidth() const;

		glm::vec3 GetLineBeginWorldSpacePosition() const;

		glm::vec3 GetLineEndWorldSpacePosition() const;

	protected:
		virtual void SyncRenderData();
	};

}
