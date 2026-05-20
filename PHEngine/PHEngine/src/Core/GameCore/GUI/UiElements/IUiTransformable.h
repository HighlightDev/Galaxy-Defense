#pragma once

#include "Core/GameCore/GUI/UiElements/Transform2D/BoundingBox2D.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorType.h"

#include <glm/vec2.hpp>

#include <memory>
#include <string>

namespace EngineCore {
class Scene;

namespace GUI {
class UiItemBase;

class IUiTransformable {
public:
    virtual size_t GetUId() const = 0;
    virtual const glm::ivec2& GetAbsoluteOrigin() const = 0;
    virtual size_t GetZOrder() const = 0;
    virtual size_t GetWidth() const = 0;
    virtual size_t GetHeight() const = 0;
    virtual glm::vec2 GetNormalizedTranslation() const = 0;
    virtual glm::vec2 GetNormalizedScale() const = 0;
    virtual std::weak_ptr<IUiTransformable> GetRootParent() const = 0;
    virtual std::weak_ptr<IUiTransformable> GetParent() const = 0;
    virtual std::string GetName() const = 0;
    virtual bool IsVisible() const = 0;
    virtual bool GetIfCanInterceptMouseInputEvents() const = 0;
    virtual BoundingBox2D<glm::ivec2> GetBoundingArea() const = 0;
    virtual bool IsTransformDirty() const = 0;

    virtual void SetAbsoluteOrigin(const glm::ivec2& translation) = 0;
    virtual void SetZOrder(const size_t z_order) = 0;
    virtual void SetWidth(const size_t width) = 0;
    virtual void SetHeight(const size_t height) = 0;
    virtual void SetIsVisible(const bool visibility) = 0;
#ifdef DEBUG
    virtual void SetIsHiddenForDebugging(const bool isHiddenForDebugging) = 0;
#endif
    virtual void SetIfCanInterceptMouseInputEvents(const bool intercepts) = 0;
    virtual void SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string& dstUiItemName) = 0;
    virtual void SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin) = 0;
    virtual void SetHorizontalCenterOffset(const int32_t offset) = 0;
    virtual void SetVerticalCenterOffset(const int32_t offset) = 0;

    virtual void RegisterUiItem(const size_t uiId, const std::string& uiItemName) = 0;
    virtual void UnregisterUiItem(const size_t uiId, const std::string& uiItemName) = 0;

    virtual std::weak_ptr<::EngineCore::Scene> GetScene() const = 0;

    virtual std::shared_ptr<IUiTransformable> TryFindChildByName(const std::string& name) const = 0;

    virtual void AddUiItem(const std::shared_ptr<::EngineCore::GUI::UiItemBase>& uiItem) = 0;
    virtual void RemoveUiItem(const std::shared_ptr<UiItemBase>& uiItem) = 0;
};
} // namespace GUI
} // namespace EngineCore