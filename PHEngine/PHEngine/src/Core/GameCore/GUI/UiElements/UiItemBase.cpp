#include "UiItemBase.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorPositionHelper.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiItemBaseLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/AnimationData.h"
#include "Core/GameCore/GUI/OverlayManagement/GuiAnimation/Animator.h"
#include "Core/GameCore/EngineObjectProperty.h"

#include <glm/gtc/matrix_transform.hpp>
#include <json/json.hpp>
#include <unordered_map>
#include <tuple>
#include <iostream>

using namespace EngineCore;
using namespace Graphics::Renderer;
using namespace EngineCore::Scripts;

namespace EngineCore
{
    namespace GUI
    {
        size_t UiItemBase::s_UIds = 0;

        UiItemBase::UiItemBase(const std::string& name)
            : mUId(s_UIds++),
              mName(name),
              mAbsoluteOrigin(),
              mNormalizedTranslation(),
              mNormalizedScale(glm::vec2(1.0)),
              mZOrder(0),
              mWidth(0),
              mHeight(0),
              mBoundingArea(),
              mAnchors(),
              mHorizontalCenterOffset(0),
              mVerticalCenterOffset(0),
              mParent(),
              mParentCanvas(),
              mChildren(),
              mIsVisible(true),
              mIsTransformDirty(false),
              mIsPropertiesShouldBeUpdatedOnRenderThread(false),
              mIsPropertiesShouldBeUpdatedOnLuaThread(false),
              mScaleProperty(std::make_shared<EngineObjectProperty<float>>(1.0f, "Scale", [this](const float newScaleValue)
                                                                           { UpdateScaleProperty(); })),
              mVerticalCenterOffsetProperty(std::make_shared<EngineObjectProperty<int32_t>>(0, "VerticalCenterOffset", [this](const int32_t verticalCenterOffset)
                                                                                            { UpdateCenterOffsetProperties(); })),
              mHorizontalCenterOffsetProperty(std::make_shared<EngineObjectProperty<int32_t>>(0, "HorizontalCenterOffset", [this](const int32_t horizontalCenterOffset)
                                                                                              { UpdateCenterOffsetProperties(); }))
        {
            mProperties.emplace("Scale", mScaleProperty);
            mProperties.emplace("VerticalCenterOffset", mVerticalCenterOffsetProperty);
            mProperties.emplace("HorizontalCenterOffset", mHorizontalCenterOffsetProperty);
        }

        void UiItemBase::Initialize()
        {
            mName = mName == std::string("") ? GetUiTypeString() + "_" + std::to_string(mUId) : mName + "_" + std::to_string(mUId);
        }

        void UiItemBase::SetParents(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent)
        {
            mParentCanvas = parentCanvas;
            mParent = parent;
            const auto &parentSp = mParent.lock();
            assert(parentSp);
            parentSp->AddUiItem(std::static_pointer_cast<UiItemBase>(shared_from_this()));
        }

        void UiItemBase::SetParents(const std::string &uiCanvasName, const std::string &uiWidgetParentName)
        {
            if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
            {
                if (const auto &sceneSp = luaScriptProcessorSp->GetInterThreadCommunicationManager().GetSceneWP().lock())
                {
                    const auto parentCanvas = sceneSp->GetUiHandler()->GetCanvasByName(uiCanvasName);
                    assert(parentCanvas);
                    mParentCanvas = parentCanvas;
                    if (uiCanvasName == uiWidgetParentName)
                    {
                        mParent = parentCanvas;
                        parentCanvas->AddUiItem(std::static_pointer_cast<UiItemBase>(shared_from_this()));
                    }
                    else
                    {
                        const auto parent = parentCanvas->TryFindHierarchyChildByName(uiWidgetParentName);
                        assert(parent);
                        mParent = parent;
                        parent->AddUiItem(std::static_pointer_cast<UiItemBase>(shared_from_this()));
                    }
                }
            }
        }

        void UiItemBase::SetIsSceneProxyReady(const bool isSceneProxyReady)
        {
            LogInfo("UiItemBase::SetIsSceneProxyReady => name: ", mName, ", readiness value: ", isSceneProxyReady);
            mIsSceneProxyReady.store(isSceneProxyReady, std::memory_order::memory_order_seq_cst);
        }

        void UiItemBase::SetIsLuaProxyReady(const bool isLuaProxyReady)
        {
            LogInfo("UiItemBase::SetIsLuaProxyReady => name: ", mName, ", readiness value: ", isLuaProxyReady);
            mIsLuaProxyReady.store(isLuaProxyReady, std::memory_order::memory_order_seq_cst);
        }

        std::weak_ptr<Scene> UiItemBase::GetScene() const
        {
            if (const auto &parentSp = mParent.lock())
            {
                return parentSp->GetScene();
            }

            return std::weak_ptr<Scene>();
        }

        const std::weak_ptr<UiCanvas> &UiItemBase::GetParentCanvas() const
        {
            return mParentCanvas;
        }

        const glm::ivec2 &UiItemBase::GetAbsoluteOrigin() const
        {
            return mAbsoluteOrigin;
        }

        size_t UiItemBase::GetZOrder() const
        {
            return mZOrder;
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

        void UiItemBase::SetAbsoluteOrigin(const glm::ivec2 &transform)
        {
            if (!CheckSimilarityIVec2(mAbsoluteOrigin, transform))
            {
                mAbsoluteOrigin = transform;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetZOrder(const size_t zOrder)
        {
            if (mZOrder != zOrder)
            {
                mZOrder = zOrder;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        void UiItemBase::SetWidth(const size_t width)
        {
            assert(width > 0);
            if (mWidth != width)
            {
                mWidth = width;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetHeight(const size_t height)
        {
            assert(height > 0);
            if (mHeight != height)
            {
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
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
                SetChildrenIsVisible(mIsVisible);
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        void UiItemBase::SetChildrenIsVisible(const bool isVisible)
        {
            for (const auto &child : mChildren)
            {
                child->SetIsVisible(isVisible);
            }
        }

        void UiItemBase::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName)
        {
            assert(dstUiItemName != GetName());
            assert(UiAnchorPositionHelper::CheckIsAnchorBindingValid(srcAnchor, dstAnchor)); // Wrong anchor binding. srcAnchor
            assert(TryFindAncestryUiItem(dstUiItemName));                                    // Try to anchor to ui item which is not parent or sibling

            if (mAnchors.count(srcAnchor))
            {
                const auto &anchorData = mAnchors.at(srcAnchor);
                if (anchorData.GetDstAnchor() != dstAnchor || anchorData.GetDstUiItemName() != dstUiItemName)
                {
                    UiAnchorData anchorData;
                    anchorData.SetDstAnchor(dstAnchor);
                    anchorData.SetDstUiItemName(dstUiItemName);
                    mAnchors[srcAnchor] = anchorData;
                    SetIsTransformDirty(true);
                }
            }
            else
            {
                UiAnchorData anchorData;
                anchorData.SetDstAnchor(dstAnchor);
                anchorData.SetDstUiItemName(dstUiItemName);
                mAnchors[srcAnchor] = anchorData;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetAnchorMargin(const eUiAnchor anchor, const int32_t anchorMargin)
        {
            assert(mAnchors.count(anchor) && mAnchors.at(anchor).GetDstAnchor() != eUiAnchor::NONE);
            if (mAnchors.at(anchor).GetSrcAnchorMargin() != anchorMargin)
            {
                mAnchors[anchor].SetSrcAnchorMargin(anchorMargin);
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetHorizontalCenterOffset(const int32_t offset)
        {
            if (mHorizontalCenterOffset != offset)
            {
                mHorizontalCenterOffset = offset;
                SetIsTransformDirty(true);
            }
        }

        void UiItemBase::SetVerticalCenterOffset(const int32_t offset)
        {
            if (mVerticalCenterOffset != offset)
            {
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
            assert(std::string("") != mName);
            return mName;
        }

        BoundingBox2D<glm::ivec2> UiItemBase::GetBoundingArea() const
        {
            return mBoundingArea;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindChildByName(const std::string &name) const
        {
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [name](const auto &child)
                                              { return child->GetName() == name; });
            return foundIt != mChildren.end() ? (*foundIt) : nullptr;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindHierarchyChildByName(const std::string &name) const
        {
            for (const auto &child : mChildren)
            {
                if (child->GetName() == name)
                {
                    return child;
                }
                else if (const auto foundChild = child->TryFindHierarchyChildByName(name))
                {
                    return foundChild;
                }
            }
            return nullptr;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindHierarchyChildByUId(const uint32_t uid) const
        {
            for (const auto &child : mChildren)
            {
                if (child->GetUId() == uid)
                {
                    return child;
                }
                else if (const auto foundChild = child->TryFindHierarchyChildByUId(uid))
                {
                    return foundChild;
                }
            }
            return nullptr;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindAncestryUiItem(const std::string &name) const
        {
            std::shared_ptr<IUiTransformable> result;
            auto parentWp = mParent;

            while (const auto &parentSp = parentWp.lock())
            {
                if (parentSp->GetName() == name)
                {
                    return parentSp;
                }
                else if (const auto &child = parentSp->TryFindChildByName(name))
                {
                    return child;
                }

                parentWp = parentSp->GetParent();
            }

            return result;
        }

        std::weak_ptr<IUiTransformable> UiItemBase::GetRootParent() const
        {
            const std::shared_ptr<IUiTransformable> &parent = mParent.lock();
            if (parent)
            {
                const auto &parentSp = parent->GetRootParent().lock();
                if (parentSp)
                {
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

        void UiItemBase::SetIsPropertiesShouldBeUpdatedOnLuaThread(const bool update)
        {
            mIsPropertiesShouldBeUpdatedOnLuaThread = update;
        }

        void UiItemBase::SetMouseInputReceiver(const std::shared_ptr<IUiMouseInputReceivable> &inputReceiver)
        {
            const auto &canvasSp = mParentCanvas.lock();
            assert(canvasSp);
            assert(canvasSp->IsInputSystemInitialized());
            mMouseInputReceiver = inputReceiver;
        }

        void UiItemBase::RebuildNormalizedTransform()
        {
            if (const auto &rootParentSp = GetRootParent().lock())
            {
                const auto rootWidth = rootParentSp->GetWidth();
                const auto rootHeight = rootParentSp->GetHeight();
                const auto rootOrigin = rootParentSp->GetAbsoluteOrigin();
                assert(rootWidth != 0 && rootHeight != 0);
                mNormalizedTranslation = glm::vec2((static_cast<float>(mAbsoluteOrigin.x) / static_cast<float>(rootWidth)) - (static_cast<float>(rootOrigin.x) / static_cast<float>(rootWidth)),
                                                   (static_cast<float>(mAbsoluteOrigin.y) / static_cast<float>(rootHeight)) - (static_cast<float>(rootOrigin.y) / static_cast<float>(rootHeight)));

                mNormalizedScale = glm::vec2(static_cast<float>(mWidth) / static_cast<float>(rootWidth),
                                             static_cast<float>(mHeight) / static_cast<float>(rootHeight));

                LogInfo("UiItemBase::RebuildNormalizedTransform => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin,
                        " mWidth: ", mWidth, " mHeight: ", mHeight, " rootWidth: ", rootWidth, " rootHeight: ", rootHeight);

                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        void UiItemBase::RebuildBoundingArea()
        {
            const auto halfExtent = glm::ivec2(mWidth / 2, mHeight / 2);
            mBoundingArea = BoundingBox2D(mAbsoluteOrigin + halfExtent, halfExtent);
            LogInfo("UiItemBase::RebuildBoundingArea => uid: ", mUId, " bounding box: ", mBoundingArea);
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
            if (mAnchors.count(eUiAnchor::HORIZONTAL_CENTER))
            {
                assert(mWidth != 0); // don't forget to set the width in addition to the horizontal center alignment settings
                const auto &dstAnchor = mAnchors.at(eUiAnchor::HORIZONTAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.GetDstUiItemName());
                assert(dstAnchoringUiItem);
                const auto &dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
                mAbsoluteOrigin.x = dstBoundingArea.GetOrigin().x - (mWidth / 2) + mHorizontalCenterOffset;
            }
            else
            {
                if (mAnchors.count(eUiAnchor::LEFT) && mAnchors.count(eUiAnchor::RIGHT))
                {
                    const auto &leftAnchor = mAnchors.at(eUiAnchor::LEFT);
                    const auto &rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
                    std::shared_ptr<IUiTransformable> leftAnchorUiItem, rightAnchorUiItem;
                    if (leftAnchor.GetDstUiItemName() == rightAnchor.GetDstUiItemName())
                    {
                        leftAnchorUiItem = rightAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                    }
                    else
                    {
                        leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                        rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.GetDstUiItemName());
                    }

                    assert(leftAnchorUiItem && rightAnchorUiItem);
                    const auto &leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();
                    const auto &rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

                    int32_t originX = 0, width = 0;

                    if (eUiAnchor::LEFT == leftAnchor.GetDstAnchor())
                    {
                        originX = leftAnchorUiItemBoundingArea.GetMin().x;
                    }
                    else if (eUiAnchor::RIGHT == leftAnchor.GetDstAnchor())
                    {
                        originX = leftAnchorUiItemBoundingArea.GetMax().x;
                    };

                    if (eUiAnchor::LEFT == rightAnchor.GetDstAnchor())
                    {
                        width = (rightAnchorUiItemBoundingArea.GetMin().x - originX) - (rightAnchor.GetSrcAnchorMargin() + leftAnchor.GetSrcAnchorMargin());
                    }
                    else if (eUiAnchor::RIGHT == rightAnchor.GetDstAnchor())
                    {
                        width = (rightAnchorUiItemBoundingArea.GetMax().x - originX) - (rightAnchor.GetSrcAnchorMargin() + leftAnchor.GetSrcAnchorMargin());
                    }

                    mAbsoluteOrigin.x = originX + leftAnchor.GetSrcAnchorMargin() + mHorizontalCenterOffset;
                    mWidth = width;
                }
                else if (mAnchors.count(eUiAnchor::LEFT))
                {
                    const auto &leftAnchor = mAnchors.at(eUiAnchor::LEFT);
                    const auto &leftAnchorUiItem = TryFindAncestryUiItem(leftAnchor.GetDstUiItemName());
                    assert(leftAnchorUiItem);
                    const auto &leftAnchorUiItemBoundingArea = leftAnchorUiItem->GetBoundingArea();

                    const int32_t originX = eUiAnchor::LEFT == leftAnchor.GetDstAnchor() ? leftAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == leftAnchor.GetDstAnchor() ? leftAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                                   : 0;
                    mAbsoluteOrigin.x = originX + leftAnchor.GetSrcAnchorMargin() + mHorizontalCenterOffset;
                }
                else if (mAnchors.count(eUiAnchor::RIGHT))
                {
                    const auto &rightAnchor = mAnchors.at(eUiAnchor::RIGHT);
                    const auto &rightAnchorUiItem = TryFindAncestryUiItem(rightAnchor.GetDstUiItemName());
                    assert(rightAnchorUiItem);

                    const auto &rightAnchorUiItemBoundingArea = rightAnchorUiItem->GetBoundingArea();

                    const int32_t anchorOriginX = eUiAnchor::LEFT == rightAnchor.GetDstAnchor() ? rightAnchorUiItemBoundingArea.GetMin().x : eUiAnchor::RIGHT == rightAnchor.GetDstAnchor() ? rightAnchorUiItemBoundingArea.GetMax().x
                                                                                                                                                                                            : 0;
                    mAbsoluteOrigin.x = anchorOriginX - mWidth - rightAnchor.GetSrcAnchorMargin() + mHorizontalCenterOffset;
                }
            }
            LogInfo("UiItemBase::CalculateHorizontalAnchorPositions => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin, " mWidth: ", mWidth, " mHeight: ", mHeight);
        }

        void UiItemBase::CalculateVerticalAnchorPositions()
        {
            if (mAnchors.count(eUiAnchor::VERTICAL_CENTER))
            {
                assert(mHeight != 0); // don't forget to set the height in addition to the horizontal center alignment settings
                const auto &dstAnchor = mAnchors.at(eUiAnchor::VERTICAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.GetDstUiItemName());
                assert(dstAnchoringUiItem);
                const auto &dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
                mAbsoluteOrigin.y = dstBoundingArea.GetOrigin().y - (mHeight / 2) + mVerticalCenterOffset;
            }
            else
            {
                if (mAnchors.count(eUiAnchor::BOTTOM) && mAnchors.count(eUiAnchor::TOP))
                {
                    const auto &bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
                    const auto &topAnchor = mAnchors.at(eUiAnchor::TOP);
                    std::shared_ptr<IUiTransformable> bottomAnchorUiItem, topAnchorUiItem;
                    if (bottomAnchor.GetDstUiItemName() == topAnchor.GetDstUiItemName())
                    {
                        bottomAnchorUiItem = topAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                    }
                    else
                    {
                        bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                        topAnchorUiItem = TryFindAncestryUiItem(topAnchor.GetDstUiItemName());
                    }

                    assert(bottomAnchorUiItem && topAnchorUiItem);
                    const auto &bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();
                    const auto &topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

                    int32_t originY, height;
                    if (eUiAnchor::BOTTOM == bottomAnchor.GetDstAnchor())
                    {
                        originY = bottomAnchorUiItemBoundingArea.GetMin().y;
                    }
                    else if (eUiAnchor::TOP == bottomAnchor.GetDstAnchor())
                    {
                        originY = bottomAnchorUiItemBoundingArea.GetMax().y;
                    }

                    if (eUiAnchor::BOTTOM == topAnchor.GetDstAnchor())
                    {
                        height = (topAnchorUiItemBoundingArea.GetMin().y - originY) - (topAnchor.GetSrcAnchorMargin() + bottomAnchor.GetSrcAnchorMargin());
                    }
                    else if (eUiAnchor::TOP == topAnchor.GetDstAnchor())
                    {
                        height = (topAnchorUiItemBoundingArea.GetMax().y - originY) - (topAnchor.GetSrcAnchorMargin() + bottomAnchor.GetSrcAnchorMargin());
                    }

                    mAbsoluteOrigin.y = originY + bottomAnchor.GetSrcAnchorMargin() + mVerticalCenterOffset;
                    mHeight = height;
                }
                else if (mAnchors.count(eUiAnchor::BOTTOM))
                {
                    const auto &bottomAnchor = mAnchors.at(eUiAnchor::BOTTOM);
                    const auto &bottomAnchorUiItem = TryFindAncestryUiItem(bottomAnchor.GetDstUiItemName());
                    assert(bottomAnchorUiItem);
                    const auto &bottomAnchorUiItemBoundingArea = bottomAnchorUiItem->GetBoundingArea();

                    const int32_t originY = eUiAnchor::BOTTOM == bottomAnchor.GetDstAnchor() ? bottomAnchorUiItemBoundingArea.GetMin().y : eUiAnchor::TOP == bottomAnchor.GetDstAnchor() ? bottomAnchorUiItemBoundingArea.GetMax().y
                                                                                                                                                                                         : 0;
                    mAbsoluteOrigin.y = originY + bottomAnchor.GetSrcAnchorMargin() + mVerticalCenterOffset;
                }
                else if (mAnchors.count(eUiAnchor::TOP))
                {
                    const auto &topAnchor = mAnchors.at(eUiAnchor::TOP);
                    const auto &topAnchorUiItem = TryFindAncestryUiItem(topAnchor.GetDstUiItemName());
                    assert(topAnchorUiItem);

                    const auto &topAnchorUiItemBoundingArea = topAnchorUiItem->GetBoundingArea();

                    const int32_t anchorOriginY = eUiAnchor::BOTTOM == topAnchor.GetDstAnchor() ? topAnchorUiItemBoundingArea.GetMin().y : eUiAnchor::TOP == topAnchor.GetDstAnchor() ? topAnchorUiItemBoundingArea.GetMax().y
                                                                                                                                                                                      : 0;
                    mAbsoluteOrigin.y = anchorOriginY - mHeight - topAnchor.GetSrcAnchorMargin() + mVerticalCenterOffset;
                }
            }
            LogInfo("UiItemBase::CalculateVerticalAnchorPositions => uid: ", mUId, " mAbsoluteOrigin: ", mAbsoluteOrigin, " mWidth: ", mWidth, " mHeight: ", mHeight);
        }

        void UiItemBase::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            assert(GetName() == uiItem->GetParent().lock()->GetName());

            RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
            uiItem->SetIsVisible(mIsVisible);
            SetIsTransformDirty(true);

            if (const auto &canvasSp = mParentCanvas.lock())
            {
                canvasSp->CollectChildrenWithDescendingZOrder();
            }
        }

        void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId(), uiItem->GetName());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
            uiItem->OnUnregistered();

            if (const auto &canvasSp = mParentCanvas.lock())
            {
                canvasSp->CollectChildrenWithDescendingZOrder();
            }
        }

        void UiItemBase::RegisterUiItem(const size_t uiId, const std::string &uiItemName)
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->RegisterUiItem(uiId, uiItemName);
            }
        }

        void UiItemBase::UnregisterUiItem(const size_t uiId, const std::string &uiItemName)
        {
            if (const auto &parentSp = mParent.lock())
            {
                parentSp->UnregisterUiItem(uiId, uiItemName);
            }
        }

        void UiItemBase::UnpausableTick(const float deltaTime)
        {
            if (mIsTransformDirty)
            {
                UpdateAnchorTransform();
                mIsTransformDirty = false;
            }

            if (mIsPropertiesShouldBeUpdatedOnRenderThread)
            {
                OnPropertiesShouldBeUpdatedOnRenderThread();
            }

            if (mIsPropertiesShouldBeUpdatedOnLuaThread)
            {
                OnPropertiesShouldBeUpdatedOnLuaThread();
            }

            for (const auto &child : mChildren)
            {
                child->UnpausableTick(deltaTime);
            }

            if (mAnimator)
            {
                mAnimator->UnpausableTick(deltaTime);
            }
        }

        void UiItemBase::Tick(const float deltaTime)
        {
        }

        void UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            SyncDataOnRenderThread();
        }

        void UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread()
        {
            SyncDataOnLuaThread();
        }

        std::vector<std::shared_ptr<UiItemBase>> UiItemBase::GetAllChildren() const
        {
            using returnValue_t = decltype(GetAllChildren());
            returnValue_t result;

            result.insert(result.end(), mChildren.begin(), mChildren.end());

            for (const auto &child : mChildren)
            {
                const auto grandChildren = child->GetAllChildren();
                result.insert(result.end(), grandChildren.begin(), grandChildren.end());
            }

            return result;
        }

        void UiItemBase::UpdateAnchorTransform()
        {
            RecalculateAnchorPositions();

            if (const auto &canvasSp = mParentCanvas.lock())
            {
                const auto &dependentItems = canvasSp->GetDependentByTransformChildren(GetName());
                for (const auto &dependentItem : dependentItems)
                {
                    dependentItem->UpdateDependentChildrenAnchorTransform();
                }
            }
        }

        void UiItemBase::UpdateDependentChildrenAnchorTransform()
        {
            LogInfo("UiItemBase::UpdateDependentChildrenAnchorTransform => UiItem name: ", GetName());
            UpdateAnchorTransform();
        }

        bool UiItemBase::IsTransformDependentToUiItem(const std::string &uiItemName) const
        {
            return std::any_of(mAnchors.cbegin(), mAnchors.cend(), [&uiItemName](const auto &keyValueAnchor)
                               { return keyValueAnchor.second.GetDstUiItemName() == uiItemName; });
        }

        void UiItemBase::GetDependentByTransformChildren(const std::string &nameOfRelatedUiItem, std::vector<std::shared_ptr<UiItemBase>> &affectedUiItems)
        {
            for (const auto &child : mChildren)
            {
                if (child->IsTransformDependentToUiItem(nameOfRelatedUiItem))
                {
                    affectedUiItems.emplace_back(child);
                }

                child->GetDependentByTransformChildren(nameOfRelatedUiItem, affectedUiItems);
            }
        }

        void UiItemBase::CollectAllHierarchyChildren(std::vector<std::shared_ptr<UiItemBase>> &inCollection) const
        {
            for (const auto &child : mChildren)
            {
                inCollection.emplace_back(child);
                child->CollectAllHierarchyChildren(inCollection);
            }
        }

        void UiItemBase::OnMousePositionChanged(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMousePositionChanged(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::OnMouseReleased(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMouseReleased(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::OnMousePressed(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMousePressed(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::OnMouseClicked(const glm::ivec2 &mouseCursorPosition)
        {
            if (mMouseInputReceiver)
            {
                mMouseInputReceiver->OnMouseClicked(GetBoundingArea(), mouseCursorPosition);
            }
        }

        void UiItemBase::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
            const auto &jsonObj = nlohmann::json::parse(luaJsonPropsStr);
            if (jsonObj.contains("visible"))
            {
                const auto isVisible = jsonObj["visible"].get<bool>();
                // todo: maybe something better
                if (mIsVisible != isVisible)
                {
                    mIsVisible = isVisible;
                    SetChildrenIsVisible(mIsVisible);
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("z_order"))
            {
                const auto z_order = jsonObj["z_order"].get<int32_t>();
                if (mZOrder != z_order)
                {
                    mZOrder = z_order;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("width"))
            {
                const auto width = jsonObj["width"].get<size_t>();
                assert(width > 0);
                if (mWidth != width)
                {
                    mWidth = width;
                    SetIsTransformDirty(true);
                }
            }
            if (jsonObj.contains("height"))
            {
                const auto height = jsonObj["height"].get<size_t>();
                assert(height > 0);
                if (mHeight != height)
                {
                    mHeight = height;
                    SetIsTransformDirty(true);
                }
            }
            if (jsonObj.contains("horizontalCenterOffset"))
            {
                const auto horizontalCenterOffset = jsonObj["horizontalCenterOffset"].get<int32_t>();
                if (mHorizontalCenterOffset != horizontalCenterOffset)
                {
                    mHorizontalCenterOffset = horizontalCenterOffset;
                    SetIsTransformDirty(true);
                }
            }
            if (jsonObj.contains("verticalCenterOffset"))
            {
                const auto verticalCenterOffset = jsonObj["verticalCenterOffset"].get<int32_t>();
                if (mVerticalCenterOffset != verticalCenterOffset)
                {
                    mVerticalCenterOffset = verticalCenterOffset;
                    SetIsTransformDirty(true);
                }
            }
            if (jsonObj.contains("anchors"))
            {
                const auto anchorsMap = jsonObj["anchors"];
                uint8_t srcAnchorValue = static_cast<uint8_t>(eUiAnchor::LEFT);
                for (auto it = anchorsMap.begin(); it != anchorsMap.end(); ++it)
                {
                    const auto &srcAnchorMap = it.value();
                    const auto dstAnchorValue = srcAnchorMap["dstAnchor"].get<uint8_t>();
                    const auto dstUiItemWidgetName = srcAnchorMap["dstUiItemWidgetName"].get<std::string>();
                    const auto srcAnchorMargin = srcAnchorMap["srcAnchorMargin"].get<int32_t>();

                    const auto srcAnchor = static_cast<eUiAnchor>(srcAnchorValue);
                    const auto dstAnchor = static_cast<eUiAnchor>(dstAnchorValue);

                    if (eUiAnchor::NONE != dstAnchor)
                    {
                        assert(dstUiItemWidgetName != "");
                        SetAnchor(srcAnchor, dstAnchor, dstUiItemWidgetName);
                        SetAnchorMargin(srcAnchor, srcAnchorMargin);
                    }
                    ++srcAnchorValue;
                }
            }
        }

        void UiItemBase::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::SyncDataOnRenderThread");
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &canvasSp = GetParentCanvas().lock())
                    {
                        SetIsPropertiesShouldBeUpdatedOnRenderThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneSp, myUId = GetUId(), canvasUId = canvasSp->GetUId(), isVisible = mIsVisible, zOrder = mZOrder, normTranslation = mNormalizedTranslation, normScale = mNormalizedScale, width = mWidth, height = mHeight]()
                                                                                            {
                            if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                            {
                                const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                                if (uiSceneProxy)
                                {
                                    uiSceneProxy->SetIsVisible(isVisible);
                                    uiSceneProxy->SetZOrder(zOrder);
                                    uiSceneProxy->SetTransform(normTranslation, normScale);
                                    uiSceneProxy->SetWidthHeightPixels(glm::ivec2(static_cast<int32_t>(width), static_cast<int32_t>(height)));
                                } 
                            } });
                    }
                }
            }
        }

        void UiItemBase::SyncDataOnLuaThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::SyncDataOnLuaThread");
            if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                    {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [luaScriptProcessorSp, luaProxyId = GetLuaProxyId(), visible = mIsVisible, zorder = mZOrder, width = mWidth, height = mHeight, horizontalOffset = mHorizontalCenterOffset, verticalOffset = mVerticalCenterOffset, anchorsMap = mAnchors]()
                                                                                         {
                        if (const auto &uiItemBaseLuaProxy = std::static_pointer_cast<UiItemBaseLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                        {
                            uiItemBaseLuaProxy->SetIsVisible_FromGameThread(visible);
                            uiItemBaseLuaProxy->SetZOrder_FromGameThread(zorder);
                            uiItemBaseLuaProxy->SetWidth_FromGameThread(width);
                            uiItemBaseLuaProxy->SetHeight_FromGameThread(height);
                            uiItemBaseLuaProxy->SetHorizontalCenterOffset_FromGameThread(horizontalOffset);
                            uiItemBaseLuaProxy->SetVerticalCenterOffset_FromGameThread(verticalOffset);
                            for (const auto& anchor : anchorsMap) {
                                uiItemBaseLuaProxy->SetAnchor_FromGameThread(anchor.first, anchor.second);
                            }
                        } });
                    }
                }
            }
        }

        void UiItemBase::InitLuaProxy(const std::shared_ptr<Scene> &sceneSp)
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::InitLuaProxy");
            assert(sceneSp);
            mIsPendingToAddLuaProxy = false;
            const auto &luaProxy = ReplicateLuaProxy();
            luaProxy->SetSceneWp(sceneSp);
            luaProxy->SetLuaScriptProcessor(GetLuaScriptProcessorWp());

            sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::PUSH_ANYWAY, GetUId(), functionId, [this, luaScriptProcessorWp = GetLuaScriptProcessorWp(), luaProxy]
                                                                             {
                    if (const auto &luaProcessorSp = luaScriptProcessorWp.lock())
                    {
                    luaProcessorSp->AddLuaProxy(luaProxy);
                    SetIsLuaProxyReady(true);
                    } });
        }

        void UiItemBase::UpdateScaleProperty()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::UpdateScaleProperty");
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &canvasSp = GetParentCanvas().lock())
                    {
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneSp, myUId = GetUId(), canvasUId = canvasSp->GetUId(), scale = mScaleProperty->GetValue()]()
                                                                                            {
                            if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                            {
                                const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                                if (uiSceneProxy)
                                {
                                    uiSceneProxy->SetScale(scale);
                                } 
                            } });
                    }
                }
            }
        }

        void UiItemBase::UpdateCenterOffsetProperties()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::UpdateCenterOffsetProperties");
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                glm::vec2 normalizedCenterOffset;
                if (const auto &rootParentSp = GetRootParent().lock())
                {
                    const auto rootWidth = rootParentSp->GetWidth();
                    const auto rootHeight = rootParentSp->GetHeight();
                    const auto rootOrigin = rootParentSp->GetAbsoluteOrigin();
                    assert(rootWidth != 0 && rootHeight != 0);
                    const int32_t verticalCenterOffset = mVerticalCenterOffsetProperty->GetValue(), horizontalCenterOffset = mHorizontalCenterOffsetProperty->GetValue();
                    normalizedCenterOffset = glm::vec2((static_cast<float>(horizontalCenterOffset) / static_cast<float>(rootWidth)) - (static_cast<float>(rootOrigin.x) / static_cast<float>(rootWidth)),
                                                       (static_cast<float>(verticalCenterOffset) / static_cast<float>(rootHeight)) - (static_cast<float>(rootOrigin.y) / static_cast<float>(rootHeight)));
                }

                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &canvasSp = GetParentCanvas().lock())
                    {
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneSp, myUId = GetUId(), canvasUId = canvasSp->GetUId(), normalizedCenterOffset]()
                                                                                            {
                            if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                            {
                                const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                                if (uiSceneProxy)
                                {
                                    
                                    uiSceneProxy->SetCenterOffset(normalizedCenterOffset);
                                } 
                            } });
                    }
                }
            }
        }

        std::shared_ptr<EngineObjectPropertyBase> UiItemBase::GetPropertyByName(const std::string &propName) const
        {
            if (mProperties.count(propName))
            {
                return mProperties.at(propName);
            }

            return nullptr;
        }

        std::shared_ptr<Animator> UiItemBase::GetAnimator() const
        {
            return mAnimator;
        }

        void UiItemBase::CreateAnimator()
        {
            if (!mAnimator)
            {
                mAnimator = std::make_shared<Animator>(std::dynamic_pointer_cast<IAnimatable>(shared_from_this()));
            }
        }

        void UiItemBase::AddAnimation(const std::string &animationName, const AnimationData &animationData)
        {
            if (!mAnimator)
            {
                CreateAnimator();
            }
            mAnimator->AddAnimation(animationName, animationData);
        }

        void UiItemBase::CleanUp()
        {
            for (const auto& child: mChildren)
            {
                child->CleanUp();
            }
            mChildren.clear();
        }

        void UiItemBase::RemoveFromReplicators()
        {
            if (const auto &sceneSp = GetScene().lock())
            {
                sceneSp->UnregisterEngineToLuaReplicator(GetReplicatorId());
            }
        }

        void UiItemBase::RemoveLuaProxy()
        {
            if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &luaProcessorSp = GetLuaScriptProcessorWp().lock())
                {
                    luaProcessorSp->RemoveLuaProxy(GetLuaProxyId());
                }
                SetIsLuaProxyReady(false);
            }
        }
    }
}
