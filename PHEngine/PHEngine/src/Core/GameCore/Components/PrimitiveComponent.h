#pragma once
#include "SceneComponent.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"

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

	public:

      static size_t TotalPrimitiveSceneProxyIndex;

      // Id of scene proxy, this value may change
      size_t PrimitiveProxyComponentId = 0;

		PrimitiveComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

		virtual ~PrimitiveComponent();

      virtual uint64_t GetComponentType() const override;

      virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const = 0;

      virtual void UpdateRelativeMatrix(glm::mat4& parentRelativeMatrix) override;

	};

}

