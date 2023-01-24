#pragma once
#include "StaticMeshComponent.h"
#include "Core/GraphicsCore/RenderData/StaticMeshRenderData.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GameCore/Event/CameraTransformChangedEvent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedMeshPoolParameters.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using namespace Graphics::Data;
using namespace Graphics;

namespace EngineCore
{
	struct MeshComponentData;

	class RuntimeGeneratedLineComponent : public StaticMeshComponent,
										  public Event::CameraTransformChangedEvent
	{
		RuntimeGeneratedMeshPoolParameters mRtMeshParams;

		glm::vec3 mLineBeginWorldSpacePosition;

		glm::vec3 mLineEndWorldSpacePosition;

		bool mIsRenderDataDirty{false};

	protected:
		using Base = StaticMeshComponent;

	public:
		RuntimeGeneratedLineComponent(const MeshComponentData &meshComponentData,
									  const StaticMeshRenderData &renderData,
									  const RuntimeGeneratedMeshPoolParameters &rtMeshParams);

		~RuntimeGeneratedLineComponent() override;

		std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const override;

		void ProcessEvent(const typename Event::CameraTransformChangedEvent::EventData_t &data) override;

		void Tick(const float deltaTime) override;

		const RuntimeGeneratedMeshPoolParameters& GetRuntimeMeshPoolParameters() const; 

		void SetLineBeginWorldSpacePosition(const glm::vec3 &position);

		void SetLineEndWorldSpacePosition(const glm::vec3 &position);

		glm::vec3 GetLineBeginWorldSpacePosition() const;

		glm::vec3 GetLineEndWorldSpacePosition() const;

	private:
		void SyncRenderData();
	};

}
