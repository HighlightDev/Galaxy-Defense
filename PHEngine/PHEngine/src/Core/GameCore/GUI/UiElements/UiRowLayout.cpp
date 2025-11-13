#include "UiRowLayout.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiRowLayoutLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <json/json.hpp>

#include <algorithm>

using namespace EngineCore;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace GUI {

UiRowLayout::UiRowLayout(const std::string& name)
    : UiItem(name)
{
}

void UiRowLayout::UpdateAnchorTransform()
{
    UiItem::UpdateAnchorTransform();

    RecalculatePositionsForChildren();
}

void UiRowLayout::SetSpacing(const uint32_t spacing)
{
    if (mSpacing != spacing) {
        mSpacing = spacing;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

uint32_t UiRowLayout::GetSpacing() const
{
    return mSpacing;
}

void UiRowLayout::SetAlignment(const eUiRowAlignmentType alignmentType)
{
    if (mAlignmentType != alignmentType) {
        mAlignmentType = alignmentType;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

eUiRowAlignmentType UiRowLayout::GetAlignment() const
{
    return mAlignmentType;
}

void UiRowLayout::RecalculatePositionsForChildren()
{
    const uint32_t visibleChildrenCount
        = std::count_if(mChildren.cbegin(), mChildren.cend(), [](const auto& child) { return child->IsVisible(); });
    if (visibleChildrenCount && mWidth > 0 && mHeight > 0) {
        const auto childWidthZero = std::any_of(mChildren.cbegin(), mChildren.cend(), [](const auto& child) {
            return child->IsVisible() ? child->GetWidth() == 0 : false;
        });

        if (childWidthZero) {
            return;
        }

        LogInfo("UiRowLayout::RecalculatePositionsForChildren: id: ", GetUId(), " visibleChildrenCount: ", visibleChildrenCount);
        const auto spacingsCount = visibleChildrenCount - 1;
        auto potentialAccumulatedWidthOfChildren = 0;
        potentialAccumulatedWidthOfChildren = std::accumulate(
            mChildren.cbegin(), mChildren.cend(), 0, [](const int32_t total, const std::shared_ptr<UiItemBase>& child) {
                return child->IsVisible() ? static_cast<int32_t>(child->GetWidth()) + total : total;
            });

        potentialAccumulatedWidthOfChildren += spacingsCount * mSpacing;

        const uint32_t normalizedChildWidth = static_cast<uint32_t>((mWidth - (spacingsCount * mSpacing)) / visibleChildrenCount);

        int32_t childIndex = 0;
        uint32_t childPositionXCursor = mAbsoluteOrigin.x; // eUiRowAlignmentType::LEFT
        if (mAlignmentType == eUiRowAlignmentType::RIGHT) {
            childPositionXCursor = mAbsoluteOrigin.x + (mWidth - potentialAccumulatedWidthOfChildren);
        } else if (mAlignmentType == eUiRowAlignmentType::CENTER) {
            if (potentialAccumulatedWidthOfChildren < mWidth) {
                childPositionXCursor = mAbsoluteOrigin.x + (mWidth - potentialAccumulatedWidthOfChildren) / 2;
            }
        }

        for (const auto& child : mChildren) {
            if (!child->IsVisible()) {
                continue;
            }
            const auto& anchors = child->GetAnchors();
            ext_assert(anchors.size() == 0, "Ui widget cannot have anchors inside layout widget.");

            const auto childWidth = (potentialAccumulatedWidthOfChildren > mWidth) ? normalizedChildWidth : child->GetWidth();

            child->SetWidth(childWidth);
            const auto positionY
                = child->GetHeight() > mHeight ? mAbsoluteOrigin.y : mAbsoluteOrigin.y + ((mHeight - child->GetHeight()) * 0.5);
            child->SetAbsoluteOrigin(glm::ivec2(childPositionXCursor, positionY));
            childPositionXCursor += childWidth + (spacingsCount > 0 ? mSpacing : 0);
        }
    }
}

void UiRowLayout::UnpausableTick(const float deltaTimeSec)
{
    const auto childTransformDirty = std::any_of(mChildren.cbegin(), mChildren.cend(), [](const auto& child) {
        return child->IsTransformDirty() || child->IsVisibleDirty();
    });

    if (childTransformDirty) {
        SetIsTransformDirty(true);
    }

    UiItem::UnpausableTick(deltaTimeSec);
}

std::shared_ptr<LuaProxy> UiRowLayout::ReplicateLuaProxy()
{
    return std::make_shared<UiRowLayoutLuaProxy>(std::static_pointer_cast<UiRowLayout>(shared_from_this()));
}

std::string UiRowLayout::GetUiTypeString() const
{
    return "UiRowLayout";
}

void UiRowLayout::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItem::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("spacing")) {
        const auto spacing = nlohmann_utilities::GetIntFromJson(jsonObj, "spacing");
        if (spacing != mSpacing) {
            mSpacing = spacing;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("alignment")) {
        const auto alignmentType = static_cast<eUiRowAlignmentType>(nlohmann_utilities::GetIntFromJson(jsonObj, "alignment"));
        if (mAlignmentType != alignmentType) {
            mAlignmentType = alignmentType;
            SetIsTransformDirty(true);
        }
    }
}

void UiRowLayout::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

    SyncDataOnLuaThread();
}

void UiRowLayout::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiRowLayout::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
                SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [luaScriptProcessorSp, luaProxyId = GetLuaProxyId(), spacing = mSpacing, alignment = mAlignmentType](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& layoutLuaProxy
                            = std::static_pointer_cast<UiRowLayoutLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            layoutLuaProxy->SetSpacing_FromGameThread(spacing);
                            layoutLuaProxy->SetAlignment_FromGameThread(alignment);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore
