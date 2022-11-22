#pragma once

#include "Transform2D/Transform2D.h"

#include <glm/vec2.hpp>
#include <memory>

namespace EngineCore
{
    namespace GUI
    {
        class IUiTransformable
        {
        public:
            virtual const Transform2D &GetAbsoluteOrigin() const = 0;
            virtual const Transform2D &GetRelativeOrigin() const = 0;
            virtual size_t GetZOrder() const = 0;
            virtual size_t GetWidth() const = 0;
            virtual size_t GetHeight() const = 0;
            virtual glm::vec2 GetNormalizedTranslation() const = 0;
            virtual glm::vec2 GetNormalizedScale() const = 0;
            virtual std::shared_ptr<IUiTransformable> GetRootParent() const = 0;

            virtual void SetAbsoluteOrigin(const Transform2D &transform) = 0;
            virtual void SetRelativeOrigin(const Transform2D &transform) = 0;
            virtual void SetZOrder(const size_t z_order) = 0;
            virtual void SetWidth(const size_t width) = 0;
            virtual void SetHeight(const size_t height) = 0;

            virtual void RegisterUiItem(const size_t uiId) = 0;
            virtual void UnregisterUiItem(const size_t uiId) = 0;

            virtual void UpdateSortedChildren() = 0;
        };
    }
}