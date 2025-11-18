#include "UiGridLayout.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiGridLayoutLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <glm/gtc/matrix_transform.hpp>
#include <json/json.hpp>

#include <algorithm>

using namespace EngineCore;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace GUI {

UiGridLayout::UiGridLayout(const std::string& name)
    : UiItem(name)
{
}

void UiGridLayout::UpdateAnchorTransform()
{
    UiItem::UpdateAnchorTransform();

    RecalculatePositionsForChildren();
}

void UiGridLayout::SetHorizontalSpacing(const uint32_t spacing)
{
    if (mHorizontalSpacing != spacing) {
        mHorizontalSpacing = spacing;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

uint32_t UiGridLayout::GetHorizontalSpacing() const
{
    return mHorizontalSpacing;
}

void UiGridLayout::SetVerticalSpacing(const uint32_t spacing)
{
    if (mVerticalSpacing != spacing) {
        mVerticalSpacing = spacing;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

uint32_t UiGridLayout::GetVerticalSpacing() const
{
    return mVerticalSpacing;
}

void UiGridLayout::SetColumnsCount(const uint32_t count)
{
    if (mColumnsCount != count) {
        mColumnsCount = count;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

uint32_t UiGridLayout::GetColumnsCount() const
{
    return mColumnsCount;
}

void UiGridLayout::SetRowsCount(const uint32_t count)
{
    if (mRowsCount != count) {
        mRowsCount = count;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
        SetIsTransformDirty(true);
    }
}

uint32_t UiGridLayout::GetRowsCount() const
{
    return mRowsCount;
}

void UiGridLayout::RecalculatePositionsForChildren()
{
    const uint32_t visibleChildrenCount
        = std::count_if(mChildren.cbegin(), mChildren.cend(), [](const auto& child) { return child->IsVisible(); });

    const auto maxChildrenCount = mColumnsCount * mRowsCount;
    const auto spacingsCount = visibleChildrenCount - 1;

    if (visibleChildrenCount && mWidth && mHeight) {

        const auto childWidthZero = std::any_of(mChildren.cbegin(), mChildren.cend(), [](const auto& child) {
            return child->IsVisible() ? child->GetWidth() == 0 : false;
        });

        if (childWidthZero) {
            return;
        }

        const int32_t maxPotentialWidth = GetGridPotentialWidth() + (spacingsCount * mHorizontalSpacing);
        const int32_t maxPotentialHeight = GetGridPotentialHeight() + (spacingsCount * mVerticalSpacing);

        const uint32_t normalizedChildWidth
            = static_cast<uint32_t>((mWidth - (spacingsCount * mHorizontalSpacing)) / mColumnsCount);
        const uint32_t normalizedChildHeight = static_cast<uint32_t>((mHeight - (spacingsCount * mVerticalSpacing)) / mRowsCount);

        int32_t cursorX = mAbsoluteOrigin.x;
        int32_t cursorY = mAbsoluteOrigin.y;
        int32_t childIndex = 0;

        int32_t prevRowIndex = 0;
        for (const auto& child : mChildren) {
            if (not child->IsVisible()) {
                continue;
            }
            if (childIndex > maxChildrenCount) {
                break;
            }

            const auto& anchors = child->GetAnchors();
            ext_assert(anchors.size() == 0, "Ui widget cannot have anchors inside layout widget.");

            const int32_t columnIndex = childIndex % mColumnsCount;
            const int32_t rowIndex = childIndex / mColumnsCount;

            const auto childWidth = (maxPotentialWidth > mWidth) ? normalizedChildWidth : child->GetWidth();
            const auto childHeight = (maxPotentialHeight > mHeight) ? normalizedChildHeight : child->GetHeight();

            cursorX = columnIndex != 0 ? cursorX + childWidth + mHorizontalSpacing : mAbsoluteOrigin.x;

            if (rowIndex != 0 && rowIndex != prevRowIndex) {
                cursorY += maxPotentialHeight > mHeight ? normalizedChildHeight + mVerticalSpacing
                                                        : GetRowHeight(prevRowIndex) + mVerticalSpacing;
                prevRowIndex = rowIndex;
            }

            child->SetWidth(childWidth);
            child->SetHeight(childHeight);
            child->SetAbsoluteOrigin(glm::ivec2(cursorX, cursorY));

            ++childIndex;
        }
    }
}

int32_t UiGridLayout::GetGridPotentialWidth() const
{
    const auto maxChildrenCount = mColumnsCount * mRowsCount;
    int32_t maxRowWidth = 0;
    for (uint32_t childIndex = 0; childIndex < mChildren.size(); childIndex += mColumnsCount) {
        if (childIndex >= maxChildrenCount) {
            break;
        }

        const int32_t currRowWidth = std::accumulate(
            mChildren.cbegin() + childIndex,
            mChildren.cbegin() + std::min(childIndex + mColumnsCount, static_cast<uint32_t>(mChildren.size())),
            0,
            [](const int32_t total, const std::shared_ptr<UiItemBase>& child) {
                return child->IsVisible() ? static_cast<int32_t>(child->GetWidth()) + total : total;
            });

        maxRowWidth = std::max(maxRowWidth, currRowWidth);
    }
    return maxRowWidth;
}

int32_t UiGridLayout::GetGridPotentialHeight() const
{
    const auto maxChildrenCount = mColumnsCount * mRowsCount;
    int32_t maxColumnHeight = 0;
    for (uint32_t childIndex = 0; childIndex < mChildren.size(); childIndex += mRowsCount) {
        if (childIndex >= maxChildrenCount) {
            break;
        }

        const int32_t currColumnHeight = std::accumulate(
            mChildren.cbegin() + childIndex,
            mChildren.cbegin() + std::min(childIndex + mRowsCount, static_cast<uint32_t>(mChildren.size())),
            0,
            [](const int32_t total, const std::shared_ptr<UiItemBase>& child) {
                return child->IsVisible() ? static_cast<int32_t>(child->GetHeight()) + total : total;
            });

        maxColumnHeight = std::max(maxColumnHeight, currColumnHeight);
    }
    return maxColumnHeight;
}

int32_t UiGridLayout::GetRowHeight(const uint32_t rowIndex) const
{
    const auto maxChildrenCount = mColumnsCount * mRowsCount;
    int32_t rowHeight = 0;
    const uint32_t startChildIndex = rowIndex * mColumnsCount;
    if (startChildIndex >= maxChildrenCount) {
        return rowHeight;
    }

    for (uint32_t childIndex = startChildIndex;
         childIndex < std::min(startChildIndex + mColumnsCount, static_cast<uint32_t>(mChildren.size()));
         ++childIndex) {
        const auto& child = mChildren[childIndex];
        if (child->IsVisible()) {
            rowHeight = std::max(rowHeight, static_cast<int32_t>(child->GetHeight()));
        }
    }
    return rowHeight;
}

void UiGridLayout::UnpausableTick(const float deltaTimeSec)
{
    const auto childTransformDirty = std::any_of(mChildren.cbegin(), mChildren.cend(), [](const auto& child) {
        return child->IsTransformDirty() || child->IsVisibleDirty();
    });

    if (childTransformDirty) {
        SetIsTransformDirty(true);
    }

    UiItem::UnpausableTick(deltaTimeSec);
}

std::shared_ptr<LuaProxy> UiGridLayout::ReplicateLuaProxy()
{
    return std::make_shared<UiGridLayoutLuaProxy>(std::static_pointer_cast<UiGridLayout>(shared_from_this()));
}

std::string UiGridLayout::GetUiTypeString() const
{
    return "UiGridLayout";
}

void UiGridLayout::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItem::SyncFromLuaJsonProperties(luaJsonPropsStr);

    const auto& jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("horizontal_spacing")) {
        const auto spacing = nlohmann_utilities::GetIntFromJson(jsonObj, "horizontal_spacing");
        if (spacing != mHorizontalSpacing) {
            mHorizontalSpacing = spacing;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("vertical_spacing")) {
        const auto spacing = nlohmann_utilities::GetIntFromJson(jsonObj, "vertical_spacing");
        if (spacing != mVerticalSpacing) {
            mVerticalSpacing = spacing;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("columns_count")) {
        const auto count = nlohmann_utilities::GetIntFromJson(jsonObj, "columns_count");
        if (mColumnsCount != count) {
            mColumnsCount = count;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("rows_count")) {
        const auto count = nlohmann_utilities::GetIntFromJson(jsonObj, "rows_count");
        if (mRowsCount != count) {
            mRowsCount = count;
            SetIsTransformDirty(true);
        }
    }
}

void UiGridLayout::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

    SyncDataOnLuaThread();
}

void UiGridLayout::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiGridLayout::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
                SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [luaScriptProcessorSp,
                     luaProxyId = GetLuaProxyId(),
                     horizontalSpacing = mHorizontalSpacing,
                     verticalSpacing = mVerticalSpacing,
                     columnsCount = mColumnsCount,
                     rowsCount = mRowsCount](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& layoutLuaProxy
                            = std::static_pointer_cast<UiGridLayoutLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            layoutLuaProxy->SetHorizontalSpacing_FromGameThread(horizontalSpacing);
                            layoutLuaProxy->SetVerticalSpacing_FromGameThread(verticalSpacing);
                            layoutLuaProxy->SetColumnsCount_FromGameThread(columnsCount);
                            layoutLuaProxy->SetRowsCount_FromGameThread(rowsCount);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore
