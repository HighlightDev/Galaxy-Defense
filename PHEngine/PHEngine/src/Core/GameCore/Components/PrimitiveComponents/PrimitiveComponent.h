#pragma once
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GameCore/BoundingBox.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>

using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace Graphics::Proxy;

namespace EngineCore
{
      // Base class of all drawing components
      class PrimitiveComponent
          : public SceneComponent
      {
      protected:
            using Base = SceneComponent;

            BoundingBox mBoundingBox;

            std::shared_ptr<EngineGOProperty<bool>> mIsVisible;

      public:
            size_t SceneProxyId = 0;

            PrimitiveComponent(const std::string &gameObjectName,
                               const glm::vec3 &translation,
                               const glm::vec3 &rotation,
                               const glm::vec3 &scale,
                               const BoundingBox &boundingBox);

            virtual ~PrimitiveComponent();

            virtual void SetIsVisible(bool isVisible);

            bool IsVisible() const;

            virtual void SetIsEnabled(const bool bEnabled) override;

            virtual eComponentType GetComponentType() const override;

            virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const = 0;

            virtual void UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix) override;

            virtual void OnVisibilityChanged();

            BoundingBox GetBoundingBox() const;

            BoundingBox GetTransformedBoundingBox() const;
      };
}
