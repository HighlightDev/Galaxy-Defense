#pragma once
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/SceneProxy/PrimitiveSceneProxy.h"
#include "Core/GameCore/BoundingBox3D.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <atomic>

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

            BoundingBox3D mBoundingBox;

            int32_t mSceneProxyId{0};

            std::shared_ptr<EngineObjectProperty<bool>> mIsVisible;

            int32_t mSortOrderValue{0};

            std::atomic<bool> bIsSceneProxyReady{false};
            bool bIsEnabledStateDirty{false};
            bool bIsVisibleStateDirty{false};
            bool bIsSortOrderStateDirty{false};
      public:
            PrimitiveComponent(const std::string &gameObjectName,
                               const glm::vec3 &translation,
                               const glm::vec3 &rotation,
                               const glm::vec3 &scale);

            ~PrimitiveComponent() override;

            void SetSceneProxyId(const int32_t proxyId);

            int32_t GetSceneProxyId() const;

            void SetIsSceneProxyReady(const bool isReady);

            bool IsSceneProxyReady() const;

            virtual void SetIsVisible(bool isVisible);

            void Tick(const float deltaTime) override;

            bool IsVisible() const;

            void SetIsEnabled(const bool bEnabled) override;

            eComponentType GetComponentType() const override;

            virtual std::shared_ptr<PrimitiveSceneProxy> CreateSceneProxy() const = 0;

            void UpdateRelativeMatrix(const glm::mat4 &parentRelativeMatrix) override;

            void SetBoundingBox(const BoundingBox3D &boundingBox);

            void SetSortOrderValue(const int32_t orderValue);

            int32_t GetSortOrderValue() const;

            BoundingBox3D GetBoundingBox() const;

            BoundingBox3D GetTransformedBoundingBox() const;

      private:
            void SyncRenderData();
      };
}
