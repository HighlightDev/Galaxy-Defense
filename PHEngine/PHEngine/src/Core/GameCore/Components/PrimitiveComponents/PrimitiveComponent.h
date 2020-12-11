#pragma once
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GameCore/BoundingBox.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>

using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace Graphics::Proxy;

namespace Game
{
	// Base class of all drawing components
	class PrimitiveComponent :
		public SceneComponent
	{
	protected:

		using Base = SceneComponent;

      BoundingBox mBoundingBox;

	public:

      size_t SceneProxyId = 0;

		PrimitiveComponent(const std::string& gameObjectName, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

		virtual ~PrimitiveComponent();

      virtual ComponentType GetComponentType() const override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const = 0;

      virtual void UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix) override;

      virtual void OnVisibilityChanged();

      BoundingBox GetBoundingBox() const;

      BoundingBox GetTransformedBoundingBox() const;

	};

}

