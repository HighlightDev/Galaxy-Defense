#pragma once

#include <glm/vec2.hpp>
#include <memory>

namespace EngineCore
{
    class Scene;

    namespace GUI
    {
        class IUiTransformable
        {
        public:
            virtual size_t GetUId() const = 0;
            virtual const glm::ivec2 &GetAbsoluteOrigin() const = 0;
            virtual size_t GetZOrder() const = 0;
            virtual size_t GetWidth() const = 0;
            virtual size_t GetHeight() const = 0;
            virtual glm::vec2 GetNormalizedTranslation() const = 0;
            virtual glm::vec2 GetNormalizedScale() const = 0;
            virtual std::shared_ptr<IUiTransformable> GetRootParent() const = 0;
            virtual bool IsVisible() const = 0;

            virtual void SetAbsoluteOrigin(const glm::ivec2 &translation) = 0;
            virtual void SetZOrder(const size_t z_order) = 0;
            virtual void SetWidth(const size_t width) = 0;
            virtual void SetHeight(const size_t height) = 0;
            virtual void SetIsVisible(const bool visibility) = 0;

            virtual void RegisterUiItem(const size_t uiId) = 0;
            virtual void UnregisterUiItem(const size_t uiId) = 0;

            virtual std::weak_ptr<::EngineCore::Scene> GetScene() const = 0;
        };
    }
}