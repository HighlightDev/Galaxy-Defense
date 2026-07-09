#include "UiItemBase.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/EngineObjectProperty.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationSequence.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/Animator.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorPositionHelper.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiItemBaseLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <json/json.hpp>

#include <iostream>
#include <tuple>
#include <unordered_map>

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace GUI {
size_t UiItemBase::s_UIds = 0;

UiItemBase::UiItemBase(const std::string& name)
    : mUId(s_UIds++)
    , mName(name)
    , mAbsoluteOrigin()
    , mNormalizedTranslation()
    , mNormalizedScale(glm::vec2(1.0))
    , mZOrder(0)
    , mLayer(eUiLayer::INHERIT)
    , mWidth(0)
    , mHeight(0)
    , mBoundingArea()
    , mAnchors()
    , mHorizontalCenterOffset(0)
    , mVerticalCenterOffset(0)
    , mParent()
    , mParentCanvas()
    , mChildren()
    , mIsVisible(true)
    , mCanInterceptMouseInputEvents(false)
    , mIsTransformDirty(false)
    , mIsPropertiesShouldBeUpdatedOnRenderThread(false)
    , mIsPropertiesShouldBeUpdatedOnLuaThread(false)
    , mScaleProperty(std::make_shared<EngineObjectProperty<glm::vec2>>(
          glm::vec2(1.0f), "Scale", [this](const glm::vec2& newScaleValue) { UpdateScaleProperty(); }))
    , mVerticalCenterOffsetProperty(std::make_shared<EngineObjectProperty<int32_t>>(
          0, "VerticalCenterOffset", [this](const int32_t verticalCenterOffset) { UpdateCenterOffsetProperties(); }))
    , mHorizontalCenterOffsetProperty(std::make_shared<EngineObjectProperty<int32_t>>(
          0, "HorizontalCenterOffset", [this](const int32_t horizontalCenterOffset) { UpdateCenterOffsetProperties(); }))
    , mIsGuiScissorsSlave(false)
    , mIsGuiScissorsMaster(false)
    , mCanBloomBeApplied(false)
#ifdef DEBUG
    , mIsHiddenForDebugging(false)
#endif
{
    mProperties.emplace("Scale", mScaleProperty);
    mProperties.emplace("VerticalCenterOffset", mVerticalCenterOffsetProperty);
    mProperties.emplace("HorizontalCenterOffset", mHorizontalCenterOffsetProperty);
}

void UiItemBase::Initialize()
{
    mName = mName == std::string("") ? GetUiTypeString() + "_" + std::to_string(mUId) : mName;
}

void UiItemBase::SetParents(const std::weak_ptr<UiCanvas>& parentCanvas, const std::weak_ptr<IUiTransformable>& parent)
{
    mParentCanvas = parentCanvas;
    mParent = parent;
    const auto& parentSp = mParent.lock();
    ext_assert(parentSp, "UiItemBase::SetParents: parentSp is null");
    parentSp->AddUiItem(std::static_pointer_cast<UiItemBase>(shared_from_this()));
}

void UiItemBase::SetParents(const std::string& uiCanvasName, const std::string& uiWidgetParentName)
{
    if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
        if (const auto& sceneSp = luaScriptProcessorSp->GetInterThreadCommunicationManager().GetSceneWP().lock()) {
            const auto parentCanvas = sceneSp->GetUiHandler()->GetCanvasByName(uiCanvasName);
            ext_assert(parentCanvas, "UiItemBase::SetParents: parentCanvas is null, name: " + uiCanvasName);
            mParentCanvas = parentCanvas;
            if (uiCanvasName == uiWidgetParentName) {
                mParent = parentCanvas;
                parentCanvas->AddUiItem(std::static_pointer_cast<UiItemBase>(shared_from_this()));
            } else {
                const auto parent = parentCanvas->TryFindHierarchyChildByName(uiWidgetParentName);
                ext_assert(parent, "UiItemBase::SetParents: parent is null, name: " + uiWidgetParentName);
                mParent = parent;
                parent->AddUiItem(std::static_pointer_cast<UiItemBase>(shared_from_this()));
            }
        }
    }
}

void UiItemBase::SetIsSceneProxyReady(const bool isSceneProxyReady)
{
    LogInfo("UiItemBase::SetIsSceneProxyReady: name: ", mName, ", isSceneProxyReady: ", isSceneProxyReady);
    mIsSceneProxyReady.store(isSceneProxyReady, std::memory_order::seq_cst);
}

void UiItemBase::SetIsLuaProxyReady(const bool isLuaProxyReady)
{
    LogInfo(
        "UiItemBase::SetIsLuaProxyReady: name: ", mName, ", luaProxy:", GetLuaProxyId(), ", isLuaProxyReady: ", isLuaProxyReady);
    mIsLuaProxyReady.store(isLuaProxyReady, std::memory_order::seq_cst);
}

std::weak_ptr<Scene> UiItemBase::GetScene() const
{
    if (const auto& parentSp = mParent.lock()) {
        return parentSp->GetScene();
    }

    return std::weak_ptr<Scene>();
}

const std::unordered_map<eUiAnchor /*src anchor*/, UiAnchorData>& UiItemBase::GetAnchors() const
{
    return mAnchors;
}

const std::weak_ptr<UiCanvas>& UiItemBase::GetParentCanvas() const
{
    return mParentCanvas;
}

const glm::ivec2& UiItemBase::GetAbsoluteOrigin() const
{
    return mAbsoluteOrigin;
}

size_t UiItemBase::GetZOrder() const
{
    return mZOrder;
}

void UiItemBase::SetLayer(const eUiLayer layer)
{
    if (mLayer != layer) {
        mLayer = layer;
        PropagateRenderUpdateToSubtree();
    }
}

eUiLayer UiItemBase::GetLayer() const
{
    return mLayer;
}

eUiLayer UiItemBase::GetEffectiveLayer() const
{
    if (mLayer != eUiLayer::INHERIT) {
        return mLayer;
    }
    if (const auto& parentItem = std::dynamic_pointer_cast<UiItemBase>(mParent.lock())) {
        return parentItem->GetEffectiveLayer();
    }
    return eUiLayer::PANEL;
}

std::vector<int32_t> UiItemBase::GetZPath() const
{
    std::vector<int32_t> zPath;
    if (const auto& parentItem = std::dynamic_pointer_cast<UiItemBase>(mParent.lock())) {
        // Path for child = path for parent + own z-order. Сhild inherits the path from the parent and adds its own z-order to it.
        zPath = parentItem->GetZPath();
        if (mLayer != eUiLayer::INHERIT) {
            zPath[0] = static_cast<int32_t>(mLayer);
        }
    } else {
        // WIdget of the top level (parent is canvas): path starts with layer.
        zPath.push_back(static_cast<int32_t>(GetEffectiveLayer()));
    }
    zPath.push_back(static_cast<int32_t>(mZOrder));
    return zPath;
}

void UiItemBase::PropagateRenderUpdateToSubtree()
{
    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    for (const auto& child : mChildren) {
        child->PropagateRenderUpdateToSubtree();
    }
}

size_t UiItemBase::GetWidth() const
{
    return mWidth;
}

size_t UiItemBase::GetHeight() const
{
    return mHeight;
}

glm::vec2 UiItemBase::GetNormalizedTranslation() const
{
    return mNormalizedTranslation;
}

glm::vec2 UiItemBase::GetNormalizedScale() const
{
    return mNormalizedScale;
}

int32_t UiItemBase::GetHorizontalCenterOffset() const
{
    return mHorizontalCenterOffset;
}

int32_t UiItemBase::GetVerticalCenterOffset() const
{
    return mVerticalCenterOffset;
}

void UiItemBase::SetAbsoluteOrigin(const glm::ivec2& transform)
{
    if (!CheckSimilarityIVec2(mAbsoluteOrigin, transform)) {
        mAbsoluteOrigin = transform;
        SetIsTransformDirty(true);
    }
}

void UiItemBase::SetZOrder(const size_t zOrder)
{
    if (mZOrder != zOrder) {
        mZOrder = zOrder;
        PropagateRenderUpdateToSubtree();
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiItemBase::SetWidth(const size_t width)
{
    ext_assert(static_cast<bool>(width > 0UL), "Name: " + GetName());
    if (mWidth != width) {
        mWidth = width;
        SetIsTransformDirty(true);
    }
}

void UiItemBase::SetHeight(const size_t height)
{
    ext_assert(static_cast<bool>(height > 0UL), "Name: " + GetName());
    if (mHeight != height) {
        mHeight = height;
        SetIsTransformDirty(true);
    }
}

bool UiItemBase::IsVisible() const
{
    return mIsVisible;
}

void UiItemBase::SetIsVisible(const bool isVisible)
{
    if (mIsVisible != isVisible) {
        mIsVisible = isVisible;
        SetChildrenIsVisible(mIsVisible);
        SetIsTransformDirty(true); // If parent is layout - it requires recalculate layout space
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiItemBase::SetIsGuiScissorsSlave(const bool value)
{
    if (mIsGuiScissorsSlave != value) {
        mIsGuiScissorsSlave = value;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

bool UiItemBase::IsGuiScissorsSlave() const
{
    return mIsGuiScissorsSlave;
}

void UiItemBase::SetIsGuiScissorsMaster(const bool isScissorsMaster)
{
    if (mIsGuiScissorsMaster != isScissorsMaster) {
        mIsGuiScissorsMaster = isScissorsMaster;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

bool UiItemBase::IsGuiScissorsMaster() const
{
    return mIsGuiScissorsMaster;
}

bool UiItemBase::CanBloomBeApplied() const
{
    return mCanBloomBeApplied;
}

void UiItemBase::SetCanBloomBeApplied(const bool canBloomBeApplied)
{
    if (mCanBloomBeApplied != canBloomBeApplied) {
        mCanBloomBeApplied = canBloomBeApplied;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

bool UiItemBase::GetIfCanInterceptMouseInputEvents() const
{
    return mCanInterceptMouseInputEvents;
}

void UiItemBase::SetIfCanInterceptMouseInputEvents(const bool intercepts)
{
    if (mCanInterceptMouseInputEvents != intercepts) {
        mCanInterceptMouseInputEvents = intercepts;
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiItemBase::SetChildrenIsVisible(const bool isVisible)
{
    for (const auto& child : mChildren) {
        child->SetIsVisible(isVisible);
    }
}

#ifdef DEBUG
void UiItemBase::SetIsHiddenForDebugging(const bool isHiddenForDebugging)
{
    for (const auto& child : mChildren) {
        child->SetIsHiddenForDebugging(isHiddenForDebugging);
    }
}
#endif

void UiItemBase::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string& dstUiItemName)
{
    ext_assert(dstUiItemName != GetName(), "Cannot anchor to self: " + GetName());
    ext_assert(
        UiAnchorPositionHelper::CheckIsAnchorBindingValid(srcAnchor, dstAnchor),
        "Wrong anchor binding. srcAnchor: " + std::to_string(static_cast<int>(srcAnchor))
            + ", dstAnchor: " + std::to_string(static_cast<int>(dstAnchor)));
    ext_assert(TryFindAncestryUiItem(dstUiItemName), "Try to anchor to ui item which is not parent or sibling: " + dstUiItemName);

    if (mAnchors.count(srcAnchor)) {
        const auto& anchorData = mAnchors.at(srcAnchor);
        if (anchorData.GetDstAnchor() != dstAnchor || anchorData.GetDstUiItemName() != dstUiItemName) {
            UiAnchorData anchorData;
            anchorData.SetDstAnchor(dstAnchor);
            anchorData.SetDstUiItemName(dstUiItemName);
            mAnchors[srcAnchor] = anchorData;
            SetIsTransformDirty(true);
        }
    } else {
        UiAnchorData anchorData;
        anchorData.SetDstAnchor(dstAnchor);
        anchorData.SetDstUiItemName(dstUiItemName);
        mAnchors[srcAnchor] = anchorData;
        SetIsTransformDirty(true);
    }
}

void UiItemBase::SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin)
{
    ext_assert(
        mAnchors.count(anchor) && mAnchors.at(anchor).GetDstAnchor() != eUiAnchor::NONE,
        "Anchor not set for UiItem: " + GetName());
    if (mAnchors.at(anchor).GetSrcAnchorMargin() != anchorMargin) {
        mAnchors[anchor].SetSrcAnchorMargin(anchorMargin);
        SetIsTransformDirty(true);
    }
}

void UiItemBase::SetHorizontalCenterOffset(const int32_t offset)
{
    if (mHorizontalCenterOffset != offset) {
        mHorizontalCenterOffset = offset;
        SetIsTransformDirty(true);
    }
}

void UiItemBase::SetVerticalCenterOffset(const int32_t offset)
{
    if (mVerticalCenterOffset != offset) {
        mVerticalCenterOffset = offset;
        SetIsTransformDirty(true);
    }
}

size_t UiItemBase::GetUId() const
{
    return mUId;
}

std::string UiItemBase::GetName() const
{
    // Forgot to call Initialize() method
    ext_assert(std::string("") != mName, "UiItemBase::GetName: UiItem '" + mName + "' not initialized properly.");
    return mName;
}

BoundingBox2D<glm::ivec2> UiItemBase::GetBoundingArea() const
{
    return mBoundingArea;
}

std::shared_ptr<IUiTransformable> UiItemBase::TryFindChildByName(const std::string& name) const
{
    const auto foundIt
        = std::find_if(mChildren.begin(), mChildren.end(), [name](const auto& child) { return child->GetName() == name; });
    return foundIt != mChildren.end() ? (*foundIt) : nullptr;
}

std::shared_ptr<IUiTransformable> UiItemBase::TryFindHierarchyChildByName(const std::string& name) const
{
    for (const auto& child : mChildren) {
        if (child->GetName() == name) {
            return child;
        } else if (const auto foundChild = child->TryFindHierarchyChildByName(name)) {
            return foundChild;
        }
    }
    return nullptr;
}

std::shared_ptr<IUiTransformable> UiItemBase::TryFindHierarchyChildByUId(const uint32_t uid) const
{
    for (const auto& child : mChildren) {
        if (child->GetUId() == uid) {
            return child;
        } else if (const auto foundChild = child->TryFindHierarchyChildByUId(uid)) {
            return foundChild;
        }
    }
    return nullptr;
}

std::shared_ptr<IUiTransformable> UiItemBase::TryFindAncestryUiItem(const std::string& name) const
{
    std::shared_ptr<IUiTransformable> result;
    auto parentWp = mParent;

    while (const auto& parentSp = parentWp.lock()) {
        if (parentSp->GetName() == name) {
            return parentSp;
        } else if (const auto& child = parentSp->TryFindChildByName(name)) {
            return child;
        }

        parentWp = parentSp->GetParent();
    }

    return result;
}

std::weak_ptr<IUiTransformable> UiItemBase::GetRootParent() const
{
    const std::shared_ptr<IUiTransformable>& parent = mParent.lock();
    if (parent) {
        const auto& parentSp = parent->GetRootParent().lock();
        if (parentSp) {
            return parentSp;
        }
    }
    return parent;
}

std::weak_ptr<IUiTransformable> UiItemBase::GetParent() const
{
    return mParent;
}

std::shared_ptr<IUiMouseInputReceivable> UiItemBase::GetMouseInputReceiver() const
{
    return mMouseInputReceiver;
}

bool UiItemBase::IsTransformDirty() const
{
    return mIsTransformDirty;
}

void UiItemBase::SetIsTransformDirty(const bool isDirty)
{
    mIsTransformDirty = isDirty;
}

void UiItemBase::SetIsPropertiesShouldBeUpdatedOnRenderThread(const bool update)
{
    mIsPropertiesShouldBeUpdatedOnRenderThread = update;
}

bool UiItemBase::IsPropertiesShouldBeUpdatedOnRenderThread() const
{
    return mIsPropertiesShouldBeUpdatedOnRenderThread;
}

void UiItemBase::SetIsPropertiesShouldBeUpdatedOnLuaThread(const bool update)
{
    mIsPropertiesShouldBeUpdatedOnLuaThread = update;
}

void UiItemBase::SetMouseInputReceiver(const std::shared_ptr<IUiMouseInputReceivable>& inputReceiver)
{
    const auto& canvasSp = mParentCanvas.lock();
    ext_assert(canvasSp, "UiItemBase::SetMouseInputReceiver: parentCanvas is null, name: " + GetName());
    ext_assert(
        canvasSp->IsInputSystemInitialized(),
        "UiItemBase::SetMouseInputReceiver: UiInputSystem is not initialized, name: " + GetName());
    mMouseInputReceiver = inputReceiver;
}

void UiItemBase::RebuildNormalizedTransform()
{
    if (const auto& rootParentSp = GetRootParent().lock()) {
        const auto rootWidth = rootParentSp->GetWidth();
        const auto rootHeight = rootParentSp->GetHeight();
        const auto rootOrigin = rootParentSp->GetAbsoluteOrigin();
        ext_assert(
            rootWidth != 0 && rootHeight != 0,
            "UiItemBase::RebuildNormalizedTransform: rootWidth or rootHeight is zero, name: " + GetName());
        mNormalizedTranslation = glm::vec2(
            (static_cast<float>(mAbsoluteOrigin.x) / static_cast<float>(rootWidth))
                - (static_cast<float>(rootOrigin.x) / static_cast<float>(rootWidth)),
            (static_cast<float>(mAbsoluteOrigin.y) / static_cast<float>(rootHeight))
                - (static_cast<float>(rootOrigin.y) / static_cast<float>(rootHeight)));

        mNormalizedScale = glm::vec2(
            static_cast<float>(mWidth) / static_cast<float>(rootWidth),
            static_cast<float>(mHeight) / static_cast<float>(rootHeight));

        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiItemBase::RebuildBoundingArea()
{
    const auto halfExtent = glm::ivec2(mWidth / 2, mHeight / 2);
    mBoundingArea = BoundingBox2D(mAbsoluteOrigin + halfExtent, halfExtent);
}

void UiItemBase::RecalculateAnchorPositions()
{
    CalculateHorizontalAnchorPositions();
    CalculateVerticalAnchorPositions();

    RebuildBoundingArea();
    RebuildNormalizedTransform();
}

void UiItemBase::CalculateHorizontalAnchorPositions()
{
    if (mAnchors.count(eUiAnchor::HORIZONTAL_CENTER)) {
        ext_assert(
            mWidth != 0,
            "uiid: "
                + std::to_string(mUId)); // don't forget to set the width in addition to the horizontal center alignment settings
        const auto& dstAnchor = mAnchors.at(eUiAnchor::HORIZONTAL_CENTER);
        const auto& dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.GetDstUiItemName());
        ext_assert(
            dstAnchoringUiItem, "UiItemBase::CalculateHorizontalAnchorPositions: dstAnchoringUiItem is null, name: " + GetName());
        const auto& dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
        mAbsoluteOrigin.x = dstBoundingArea.GetOrigin().x - (mWidth / 2) + mHorizontalCenterOffset;
    } else {
        if (mAnchors.count(eUiAnchor::LEFT) && mAnchors.count(eUiAnchor::RIGHT)) {
            const auto& leftAnchor = mAnchors.at(eUiAnchor::LEFT);
            const auto& rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
            std::shared_ptr<IUiTransformable> leftAnchorUiItem, rightAnchorUiItem;
            if (leftAnchor.GetDstUiItemName() == rightAnchor.GetDstUiItemName()) {
                leftAnchorUiItem = rightAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
            } else {
                leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.GetDstUiItemName());
            }

            ext_assert(
                leftAnchorUiItem && rightAnchorUiItem,
                "UiItemBase::CalculateHorizontalAnchorPositions: leftAnchorUiItem or rightAnchorUiItem is null, name: "
                    + GetName());
            const auto& leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();
            const auto& rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

            int32_t originX = 0, width = 0;

            if (eUiAnchor::LEFT == leftAnchor.GetDstAnchor()) {
                originX = leftAnchorUiItemBoundingArea.GetMin().x;
            } else if (eUiAnchor::RIGHT == leftAnchor.GetDstAnchor()) {
                originX = leftAnchorUiItemBoundingArea.GetMax().x;
            };

            if (eUiAnchor::LEFT == rightAnchor.GetDstAnchor()) {
                width = (rightAnchorUiItemBoundingArea.GetMin().x - originX)
                    - (rightAnchor.GetSrcAnchorMargin() + leftAnchor.GetSrcAnchorMargin());
            } else if (eUiAnchor::RIGHT == rightAnchor.GetDstAnchor()) {
                width = (rightAnchorUiItemBoundingArea.GetMax().x - originX)
                    - (rightAnchor.GetSrcAnchorMargin() + leftAnchor.GetSrcAnchorMargin());
            }

            mAbsoluteOrigin.x = originX + leftAnchor.GetSrcAnchorMargin() + mHorizontalCenterOffset;
            mWidth = width;
        } else if (mAnchors.count(eUiAnchor::LEFT)) {
            const auto& leftAnchor = mAnchors.at(eUiAnchor::LEFT);
            const auto& leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
            ext_assert(
                leftAnchorUiItem, "UiItemBase::CalculateHorizontalAnchorPositions: leftAnchorUiItem is null, name: " + GetName());
            const auto& leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();

            const int32_t originX = eUiAnchor::LEFT == leftAnchor.GetDstAnchor()
                ? leftAnchorUiItemBoundingArea.GetMin().x
                : eUiAnchor::RIGHT == leftAnchor.GetDstAnchor() ? leftAnchorUiItemBoundingArea.GetMax().x : 0;
            mAbsoluteOrigin.x = originX + leftAnchor.GetSrcAnchorMargin() + mHorizontalCenterOffset;
        } else if (mAnchors.count(eUiAnchor::RIGHT)) {
            const auto& rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
            const auto& rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.GetDstUiItemName());
            ext_assert(
                rightAnchorUiItem,
                "UiItemBase::CalculateHorizontalAnchorPositions: rightAnchorUiItem is null, name: " + GetName());

            const auto& rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

            const int32_t anchorOriginX = eUiAnchor::LEFT == rightAnchor.GetDstAnchor()
                ? rightAnchorUiItemBoundingArea.GetMin().x
                : eUiAnchor::RIGHT == rightAnchor.GetDstAnchor() ? rightAnchorUiItemBoundingArea.GetMax().x : 0;
            mAbsoluteOrigin.x = anchorOriginX - mWidth - rightAnchor.GetSrcAnchorMargin() + mHorizontalCenterOffset;
        }
    }
}

void UiItemBase::CalculateVerticalAnchorPositions()
{
    if (mAnchors.count(eUiAnchor::VERTICAL_CENTER)) {
        ext_assert(mHeight != 0, "UiItemBase::CalculateVerticalAnchorPositions: mHeight is zero, name: " + GetName());
        const auto& dstAnchor = mAnchors.at(eUiAnchor::VERTICAL_CENTER);
        const auto& dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.GetDstUiItemName());
        ext_assert(
            dstAnchoringUiItem, "UiItemBase::CalculateVerticalAnchorPositions: dstAnchoringUiItem is null, name: " + GetName());
        const auto& dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
        mAbsoluteOrigin.y = dstBoundingArea.GetOrigin().y - (mHeight / 2) + mVerticalCenterOffset;
    } else {
        if (mAnchors.count(eUiAnchor::BOTTOM) && mAnchors.count(eUiAnchor::TOP)) {
            const auto& bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
            const auto& topAnchor = mAnchors.at(eUiAnchor::TOP);
            std::shared_ptr<IUiTransformable> bottomAnchorUiItem, topAnchorUiItem;
            if (bottomAnchor.GetDstUiItemName() == topAnchor.GetDstUiItemName()) {
                bottomAnchorUiItem = topAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
            } else {
                bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                topAnchorUiItem = TryFindAncestryUiItem(topAnchor.GetDstUiItemName());
            }

            ext_assert(
                bottomAnchorUiItem && topAnchorUiItem,
                "UiItemBase::CalculateVerticalAnchorPositions: bottomAnchorUiItem or topAnchorUiItem is null, name: "
                    + GetName());
            const auto& bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();
            const auto& topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

            int32_t originY, height;
            if (eUiAnchor::BOTTOM == bottomAnchor.GetDstAnchor()) {
                originY = bottomAnchorUiItemBoundingArea.GetMin().y;
            } else if (eUiAnchor::TOP == bottomAnchor.GetDstAnchor()) {
                originY = bottomAnchorUiItemBoundingArea.GetMax().y;
            }

            if (eUiAnchor::BOTTOM == topAnchor.GetDstAnchor()) {
                height = (topAnchorUiItemBoundingArea.GetMin().y - originY)
                    - (topAnchor.GetSrcAnchorMargin() + bottomAnchor.GetSrcAnchorMargin());
            } else if (eUiAnchor::TOP == topAnchor.GetDstAnchor()) {
                height = (topAnchorUiItemBoundingArea.GetMax().y - originY)
                    - (topAnchor.GetSrcAnchorMargin() + bottomAnchor.GetSrcAnchorMargin());
            }

            mAbsoluteOrigin.y = originY + bottomAnchor.GetSrcAnchorMargin() + mVerticalCenterOffset;
            mHeight = height;
        } else if (mAnchors.count(eUiAnchor::BOTTOM)) {
            const auto& bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
            const auto& bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
            ext_assert(
                bottomAnchorUiItem,
                "UiItemBase::CalculateVerticalAnchorPositions: bottomAnchorUiItem is null, name: " + GetName());
            const auto& bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();

            const int32_t originY = eUiAnchor::BOTTOM == bottomAnchor.GetDstAnchor()
                ? bottomAnchorUiItemBoundingArea.GetMin().y
                : eUiAnchor::TOP == bottomAnchor.GetDstAnchor() ? bottomAnchorUiItemBoundingArea.GetMax().y : 0;
            mAbsoluteOrigin.y = originY + bottomAnchor.GetSrcAnchorMargin() + mVerticalCenterOffset;
        } else if (mAnchors.count(eUiAnchor::TOP)) {
            const auto& topAnchor = mAnchors.at(eUiAnchor::TOP);
            const auto& topAnchorUiItem = TryFindAncestryUiItem(topAnchor.GetDstUiItemName());
            ext_assert(
                topAnchorUiItem, "UiItemBase::CalculateVerticalAnchorPositions: topAnchorUiItem is null, name: " + GetName());

            const auto& topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

            const int32_t anchorOriginY = eUiAnchor::BOTTOM == topAnchor.GetDstAnchor()
                ? topAnchorUiItemBoundingArea.GetMin().y
                : eUiAnchor::TOP == topAnchor.GetDstAnchor() ? topAnchorUiItemBoundingArea.GetMax().y : 0;
            mAbsoluteOrigin.y = anchorOriginY - mHeight - topAnchor.GetSrcAnchorMargin() + mVerticalCenterOffset;
        }
    }
}

void UiItemBase::AddUiItem(const std::shared_ptr<UiItemBase>& uiItem)
{
    ext_assert(
        GetName() == uiItem->GetParent().lock()->GetName(),
        "UiItemBase::AddUiItem: uiItem's parent is not this UiItem. uiItem name: " + uiItem->GetName()
            + ", parent name: " + uiItem->GetParent().lock()->GetName() + ", this name: " + GetName());

    RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
    mChildren.emplace_back(uiItem);
    uiItem->OnRegistered();
    uiItem->SetIsVisible(mIsVisible);
    SetIsTransformDirty(true);

    if (const auto& canvasSp = mParentCanvas.lock()) {
        canvasSp->CollectChildrenWithDescendingZOrder();
    }

    uiItem->OnPostRegistered();
}

void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase>& uiItem)
{
    UnregisterUiItem(uiItem->GetUId(), uiItem->GetName());
    uiItem->OnUnregistered();
    const auto it = std::remove_if(
        mChildren.begin(), mChildren.end(), [&](const auto& childUi) { return childUi->GetUId() == uiItem->GetUId(); });
    mChildren.erase(it);

    if (const auto& canvasSp = mParentCanvas.lock()) {
        canvasSp->CollectChildrenWithDescendingZOrder();
    }
}

void UiItemBase::RegisterUiItem(const size_t uiId, const std::string& uiItemName)
{
    if (const auto& parentSp = mParent.lock()) {
        parentSp->RegisterUiItem(uiId, uiItemName);
    }
}

void UiItemBase::UnregisterUiItem(const size_t uiId, const std::string& uiItemName)
{
    if (const auto& parentSp = mParent.lock()) {
        parentSp->UnregisterUiItem(uiId, uiItemName);
    }
}

void UiItemBase::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
    if (mIsTransformDirty) {
        UpdateAnchorTransform();
        mIsTransformDirty = false;
    }

    if (mIsPropertiesShouldBeUpdatedOnRenderThread) {
        const bool successfullyUpdated = OnPropertiesShouldBeUpdatedOnRenderThread();
        mIsPropertiesShouldBeUpdatedOnRenderThread = !successfullyUpdated;
    }

    if (mIsPropertiesShouldBeUpdatedOnLuaThread) {
        const bool successfullyUpdated = OnPropertiesShouldBeUpdatedOnLuaThread();
        mIsPropertiesShouldBeUpdatedOnLuaThread = !successfullyUpdated;
    }

    for (const auto& child : mChildren) {
        child->UnpausableTick(deltaTimeSec, playSpeed);
    }

    if (mAnimator) {
        mAnimator->UnpausableTick(deltaTimeSec, playSpeed);
    }

    if (mSequenceAnimator) {
        mSequenceAnimator->UnpausableTick(deltaTimeSec, playSpeed);
    }
}

void UiItemBase::Tick(const float deltaTimeSec, const float playSpeed)
{
}

bool UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    return SyncDataOnRenderThread();
}

bool UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    return SyncDataOnLuaThread();
}

std::vector<std::shared_ptr<UiItemBase>> UiItemBase::GetAllChildren() const
{
    using returnValue_t = decltype(GetAllChildren());
    returnValue_t result;

    result.insert(result.end(), mChildren.begin(), mChildren.end());

    for (const auto& child : mChildren) {
        const auto grandChildren = child->GetAllChildren();
        result.insert(result.end(), grandChildren.begin(), grandChildren.end());
    }

    return result;
}

void UiItemBase::SetIsGuiScissorsSlaveRecursive(const bool isSlave)
{
    SetIsGuiScissorsSlave(isSlave);
    for (const auto& child : mChildren) {
        child->SetIsGuiScissorsSlaveRecursive(isSlave);
    }
}

void UiItemBase::UpdateAnchorTransform()
{
    RecalculateAnchorPositions();

    if (const auto& canvasSp = mParentCanvas.lock()) {
        const auto& dependentItems = canvasSp->GetDependentByTransformChildren(GetName());
        for (const auto& dependentItem : dependentItems) {
            dependentItem->UpdateDependentChildrenAnchorTransform();
        }
    }
}

bool UiItemBase::CheckIfInterceptsMouseEvent(const glm::ivec2& currentMousePosition) const
{
    const auto& uiItemBoundingArea = GetBoundingArea();
    const auto& boundMin = uiItemBoundingArea.GetMin();
    const auto& boundMax = uiItemBoundingArea.GetMax();
    return EngineMath::TestPointInAABB(boundMin, boundMax, currentMousePosition);
}

void UiItemBase::UpdateDependentChildrenAnchorTransform()
{
    UpdateAnchorTransform();
}

bool UiItemBase::IsTransformDependentToUiItem(const std::string& uiItemName) const
{
    return std::any_of(mAnchors.cbegin(), mAnchors.cend(), [&uiItemName](const auto& keyValueAnchor) {
        return keyValueAnchor.second.GetDstUiItemName() == uiItemName;
    });
}

void UiItemBase::GetDependentByTransformChildren(
    const std::string& nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>>& affectedUiItems)
{
    for (const auto& child : mChildren) {
        if (child->IsTransformDependentToUiItem(nameOfRelatedUiItem)) {
            affectedUiItems.emplace_back(child);
        }

        child->GetDependentByTransformChildren(nameOfRelatedUiItem, affectedUiItems);
    }
}

void UiItemBase::CollectAllHierarchyChildren(std::vector<std::shared_ptr<UiItemBase>>& inCollection) const
{
    for (const auto& child : mChildren) {
        inCollection.emplace_back(child);
        child->CollectAllHierarchyChildren(inCollection);
    }
}

void UiItemBase::OnMousePositionChanged(const glm::ivec2& mouseCursorPosition)
{
    if (mMouseInputReceiver && mCanInterceptMouseInputEvents) {
        mMouseInputReceiver->OnMousePositionChanged(GetBoundingArea(), mouseCursorPosition);
    }
}

void UiItemBase::OnMouseReleased(const glm::ivec2& mouseCursorPosition)
{
    if (mMouseInputReceiver && mCanInterceptMouseInputEvents) {
        mMouseInputReceiver->OnMouseReleased(GetBoundingArea(), mouseCursorPosition);
    }
}

void UiItemBase::OnMousePressed(const glm::ivec2& mouseCursorPosition)
{
    if (mMouseInputReceiver && mCanInterceptMouseInputEvents) {
        mMouseInputReceiver->OnMousePressed(GetBoundingArea(), mouseCursorPosition);
    }
}

void UiItemBase::OnMouseClicked(const glm::ivec2& mouseCursorPosition)
{
    if (mMouseInputReceiver && mCanInterceptMouseInputEvents) {
        mMouseInputReceiver->OnMouseClicked(GetBoundingArea(), mouseCursorPosition);
    }
}

void UiItemBase::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    const auto jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("visible")) {
        const auto isVisible = jsonObj["visible"].get<bool>();
        if (mIsVisible != isVisible) {
            mIsVisible = isVisible;
#ifdef DEBUG
            SetChildrenIsVisible(mIsVisible && not mIsHiddenForDebugging);
#else
            SetChildrenIsVisible(mIsVisible);
#endif
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("intercept_mouse_input_event")) {
        const auto canIntercept = jsonObj["intercept_mouse_input_event"].get<bool>();
        if (mCanInterceptMouseInputEvents != canIntercept) {
            mCanInterceptMouseInputEvents = canIntercept;
        }
    }
    if (jsonObj.contains("layer")) {
        const auto layer = static_cast<eUiLayer>(jsonObj["layer"].get<int32_t>());
        if (mLayer != layer) {
            mLayer = layer;
            PropagateRenderUpdateToSubtree();
        }
    }
    if (jsonObj.contains("z_order")) {
        const auto z_order = jsonObj["z_order"].get<int32_t>();
        if (mZOrder != z_order) {
            mZOrder = z_order;
            PropagateRenderUpdateToSubtree();
        }
    }
    if (jsonObj.contains("width")) {
        const auto width = jsonObj["width"].get<size_t>();
        ext_assert(width > 0, "UiItemBase::SyncFromLuaJsonProperties: width is zero, name: " + GetName());
        if (mWidth != width) {
            mWidth = width;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("height")) {
        const auto height = jsonObj["height"].get<size_t>();
        ext_assert(height > 0, "UiItemBase::SyncFromLuaJsonProperties: height is zero, name: " + GetName());
        if (mHeight != height) {
            mHeight = height;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("horizontalCenterOffset")) {
        const auto horizontalCenterOffset = jsonObj["horizontalCenterOffset"].get<int32_t>();
        if (mHorizontalCenterOffset != horizontalCenterOffset) {
            mHorizontalCenterOffset = horizontalCenterOffset;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("verticalCenterOffset")) {
        const auto verticalCenterOffset = jsonObj["verticalCenterOffset"].get<int32_t>();
        if (mVerticalCenterOffset != verticalCenterOffset) {
            mVerticalCenterOffset = verticalCenterOffset;
            SetIsTransformDirty(true);
        }
    }
    if (jsonObj.contains("anchors")) {
        const auto anchorsMap = jsonObj["anchors"];
        uint8_t srcAnchorValue = static_cast<uint8_t>(eUiAnchor::LEFT);
        for (auto it = anchorsMap.begin(); it != anchorsMap.end(); ++it) {
            const auto& srcAnchorMap = it.value();
            const auto dstAnchorValue = srcAnchorMap["dstAnchor"].get<uint8_t>();
            const auto dstUiItemWidgetName = srcAnchorMap["dstUiItemWidgetName"].get<std::string>();
            const auto srcAnchorMargin = srcAnchorMap["srcAnchorMargin"].get<int32_t>();

            const auto srcAnchor = static_cast<eUiAnchor>(srcAnchorValue);
            const auto dstAnchor = static_cast<eUiAnchor>(dstAnchorValue);

            if (eUiAnchor::NONE != dstAnchor) {
                ext_assert(
                    dstUiItemWidgetName != "",
                    "UiItemBase::SyncFromLuaJsonProperties: dstUiItemWidgetName is empty, name: " + GetName());
                SetAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName);
                SetAnchorMargin(srcAnchor, srcAnchorMargin);
            }
            ++srcAnchorValue;
        }
    }
    if (jsonObj.contains("is_gui_scissors_slave")) {
        const auto isScissorsSlave = jsonObj["is_gui_scissors_slave"].get<bool>();
        if (mIsGuiScissorsSlave != isScissorsSlave) {
            mIsGuiScissorsSlave = isScissorsSlave;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("is_gui_scissors_master")) {
        const auto isScissorsMaster = jsonObj["is_gui_scissors_master"].get<bool>();
        if (mIsGuiScissorsMaster != isScissorsMaster) {
            mIsGuiScissorsMaster = isScissorsMaster;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
    if (jsonObj.contains("can_bloom_be_applied")) {
        const auto canBloomBeApplied = jsonObj["can_bloom_be_applied"].get<bool>();
        if (mCanBloomBeApplied != canBloomBeApplied) {
            mCanBloomBeApplied = canBloomBeApplied;
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        }
    }
}

bool UiItemBase::SyncDataOnRenderThread()
{
    const auto& canvasSp = GetParentCanvas().lock();
    if (!canvasSp) {
        return false;
    }
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            SetIsPropertiesShouldBeUpdatedOnRenderThread(false);
            PendingUiItemBaseUpdates updateStruct
                = {static_cast<int32_t>(canvasSp->GetUId()),
                   mIsVisible,
                   GetZPath(),
                   mNormalizedTranslation,
                   mNormalizedScale,
                   mWidth,
                   mHeight,
#ifdef DEBUG
                   mIsHiddenForDebugging,
#endif
                   mIsGuiScissorsSlave,
                   mIsGuiScissorsMaster,
                   mCanBloomBeApplied};
            sceneSp->EnqueueUiItemBaseUpdate(GetUId(), updateStruct);
        }
    }
    return mIsSceneProxyReady.load();
}

bool UiItemBase::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiItemBase::SyncDataOnLuaThread");
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
                     visible = mIsVisible,
                     interceptsMouseInputEvent = mCanInterceptMouseInputEvents,
                     zorder = mZOrder,
                     width = mWidth,
                     height = mHeight,
                     horizontalOffset = mHorizontalCenterOffset,
                     verticalOffset = mVerticalCenterOffset,
                     anchorsMap = mAnchors,
                     isHiddenForDebugging = mIsHiddenForDebugging,
                     isGuiScissorsSlave = mIsGuiScissorsSlave,
                     isGuiScissorsMaster = mIsGuiScissorsMaster,
                     canBloomBeApplied = mCanBloomBeApplied](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& uiItemBaseLuaProxy
                            = std::static_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
#ifdef DEBUG
                            uiItemBaseLuaProxy->SetIsVisible_FromGameThread(visible && not isHiddenForDebugging);
#else
                            uiItemBaseLuaProxy->SetIsVisible_FromGameThread(visible);
#endif
                            uiItemBaseLuaProxy->SetIsGuiScissorsSlave_FromGameThread(isGuiScissorsSlave);
                            uiItemBaseLuaProxy->SetIfCanInterceptMouseInputEvents_FromGameThread(interceptsMouseInputEvent);
                            uiItemBaseLuaProxy->SetCanBloomBeApplied_FromGameThread(canBloomBeApplied);
                            uiItemBaseLuaProxy->SetZOrder_FromGameThread(zorder);
                            uiItemBaseLuaProxy->SetWidth_FromGameThread(width);
                            uiItemBaseLuaProxy->SetHeight_FromGameThread(height);
                            uiItemBaseLuaProxy->SetHorizontalCenterOffset_FromGameThread(horizontalOffset);
                            uiItemBaseLuaProxy->SetVerticalCenterOffset_FromGameThread(verticalOffset);
                            for (const auto& anchor : anchorsMap) {
                                uiItemBaseLuaProxy->SetAnchor_FromGameThread(anchor.first, anchor.second);
                            }
                        }
                    });
            }
        }
        return true;
    }
    return false;
}

void UiItemBase::InitLuaProxy(const std::shared_ptr<Scene>& sceneSp)
{
    static constexpr uint64_t functionId = Hash64_CT("UiItemBase::InitLuaProxy");
    ext_assert(sceneSp, "UiItemBase::InitLuaProxy: sceneSp is null, name: " + GetName());
    mIsPendingToAddLuaProxy = false;
    const auto& luaProxy = ReplicateLuaProxy();
    luaProxy->SetSceneWp(sceneSp);
    luaProxy->SetLuaScriptProcessor(GetLuaScriptProcessorWp());

    sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
        eEnqueueJobPolicy::PUSH_ANYWAY,
        GetUId(),
        functionId,
        [this, luaScriptProcessorWp = GetLuaScriptProcessorWp(), luaProxy](
            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
            std::weak_ptr<EngineCore::Scene> sceneWp,
            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
            if (const auto& luaProcessorSp = luaScriptProcessorWp.lock()) {
                luaProcessorSp->AddLuaProxy(luaProxy);
                SetIsLuaProxyReady(true);
            }
        });
}

void UiItemBase::UpdateScaleProperty()
{
    static constexpr uint64_t functionId = Hash64_CT("UiItemBase::UpdateScaleProperty");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [sceneSp, myUId = GetUId(), canvasUId = canvasSp->GetUId(), scale = mScaleProperty->GetValue()](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& sceneRenderer
                            = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                uiSceneProxy->SetScale(scale);
                            }
                        }
                    });
            }
        }
    }
}

void UiItemBase::UpdateCenterOffsetProperties()
{
    static constexpr uint64_t functionId = Hash64_CT("UiItemBase::UpdateCenterOffsetProperties");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        glm::vec2 normalizedCenterOffset;
        if (const auto& rootParentSp = GetRootParent().lock()) {
            const auto rootWidth = rootParentSp->GetWidth();
            const auto rootHeight = rootParentSp->GetHeight();
            const auto rootOrigin = rootParentSp->GetAbsoluteOrigin();
            ext_assert(
                rootWidth != 0 && rootHeight != 0,
                "UiItemBase::UpdateCenterOffsetProperties: rootWidth or rootHeight is zero, name: " + GetName());
            const int32_t verticalCenterOffset = mVerticalCenterOffsetProperty->GetValue(),
                          horizontalCenterOffset = mHorizontalCenterOffsetProperty->GetValue();
            normalizedCenterOffset = glm::vec2(
                (static_cast<float>(horizontalCenterOffset) / static_cast<float>(rootWidth))
                    - (static_cast<float>(rootOrigin.x) / static_cast<float>(rootWidth)),
                (static_cast<float>(verticalCenterOffset) / static_cast<float>(rootHeight))
                    - (static_cast<float>(rootOrigin.y) / static_cast<float>(rootHeight)));
        }

        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [sceneSp, myUId = GetUId(), canvasUId = canvasSp->GetUId(), normalizedCenterOffset](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& sceneRenderer
                            = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {

                                uiSceneProxy->SetCenterOffset(normalizedCenterOffset);
                            }
                        }
                    });
            }
        }
    }
}

std::shared_ptr<EngineObjectPropertyBase> UiItemBase::GetPropertyByName(const std::string& propName) const
{
    if (mProperties.count(propName)) {
        return mProperties.at(propName);
    }

    return nullptr;
}

std::shared_ptr<Animator> UiItemBase::GetAnimator() const
{
    return mAnimator;
}

std::shared_ptr<::EngineCore::GUI::SequenceAnimator> UiItemBase::GetSequenceAnimator() const
{
    return mSequenceAnimator;
}

void UiItemBase::CreateAnimator()
{
    if (!mAnimator) {
        mAnimator = std::make_shared<Animator>(std::dynamic_pointer_cast<IAnimatable>(shared_from_this()));
    }
}

void UiItemBase::CreateSequenceAnimator()
{
    if (!mSequenceAnimator) {
        mSequenceAnimator = std::make_shared<SequenceAnimator>(std::dynamic_pointer_cast<IAnimatable>(shared_from_this()));
    }
}

void UiItemBase::AddAnimation(const std::string& animationName, const AnimationData& animationData)
{
    if (!mAnimator) {
        CreateAnimator();
    }
    mAnimator->AddAnimation(animationName, animationData);
}

void UiItemBase::AddSequenceAnimation(const std::string& animationName, const AnimationSequence& animationSequence)
{
    if (!mSequenceAnimator) {
        CreateSequenceAnimator();
    }
    mSequenceAnimator->AddSequenceAnimation(animationName, animationSequence);
}

void UiItemBase::CleanUp()
{
    RemoveSceneProxy();
    RemoveLuaProxy();
    RemoveFromReplicators();
    for (const auto& child : mChildren) {
        child->CleanUp();
    }
    mChildren.clear();
}

void UiItemBase::RemoveFromReplicators()
{
    if (const auto& sceneSp = GetScene().lock()) {
        sceneSp->UnregisterEngineToLuaReplicator(GetReplicatorId());
    }
}

void UiItemBase::RemoveSceneProxy()
{
    static constexpr uint64_t functionId = Hash64_CT("UiItemBase::RemoveSceneProxy");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& canvasSp = mParentCanvas.lock()) {
            if (const auto& sceneSp = GetScene().lock()) {
                if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
                    sceneRendererSp->UnregisterUiSceneProxy_OnRenderThread(GetUId(), canvasSp->GetUId());
                    SetIsSceneProxyReady(false);
                }
            }
        }
    }
}

void UiItemBase::RemoveLuaProxy()
{
    static constexpr uint64_t functionId = Hash64_CT("UiItemBase::RemoveLuaProxy");
    if (const auto& luaProcessorSp = GetLuaScriptProcessorWp().lock()) {
        luaProcessorSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
            eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
            GetUId(),
            functionId,
            [luaProcessorSp, luaProxyId = GetLuaProxyId()](const auto&, const auto&, const auto&) {
                luaProcessorSp->RemoveLuaProxy(luaProxyId);
            });
    }
}
} // namespace GUI
} // namespace EngineCore
