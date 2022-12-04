#include "UiItemBase.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/GameCore/GUI/UiElements/Transform2D/UiAnchorPositionHelper.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/LoggerExtension.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace EngineMath;
using namespace EngineCore;
using namespace Graphics::Renderer;

namespace EngineCore
{
    namespace GUI
    {

        size_t UiItemBase::s_UIds = 0;

        UiItemBase::UiItemBase(const std::weak_ptr<UiCanvas> &parentCanvas, const std::weak_ptr<IUiTransformable> &parent)
            : mUId(s_UIds++),
              mName("UiItemBase_" + std::to_string(mUId)),
              mAbsoluteOrigin(),
              mNormalizedTranslation(),
              mNormalizedScale(glm::vec2(1.0)),
              mTransformMatrix(glm::mat4(1)),
              mZOrder(0),
              mWidth(0),
              mHeight(0),
              mBoundingArea(),
              mAnchors(),
              mParent(parent),
              mParentCanvas(parentCanvas),
              mChildren(),
              mIsVisible(true)
        {
            TransformChanged();
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

        glm::mat4 UiItemBase::GetTransformMatrix() const
        {
            return mTransformMatrix;
        }

        void UiItemBase::SetAbsoluteOrigin(const glm::ivec2 &transform)
        {
            if (!CheckSimilarityIVec2(mAbsoluteOrigin, transform))
            {
                mAbsoluteOrigin = transform;
                TransformChanged();
            }
        }

        void UiItemBase::SetZOrder(const size_t zOrder)
        {
            if (mZOrder != zOrder)
            {
                mZOrder = zOrder;
                TransformChanged();
                SyncDataOnRenderThread();
            }
        }

        void UiItemBase::SetWidth(const size_t width)
        {
            if (mWidth != width)
            {
                mWidth = width;
                TransformChanged();
            }
        }

        void UiItemBase::SetHeight(const size_t height)
        {
            if (mHeight != height)
            {
                mHeight = height;
                TransformChanged();
            }
        }

        size_t UiItemBase::GetUId() const
        {
            return mUId;
        }

        std::string UiItemBase::GetName() const
        {
            return mName;
        }

        BoundingBox2D UiItemBase::GetBoundingArea() const
        {
            return mBoundingArea;
        }

        std::shared_ptr<IUiTransformable> UiItemBase::TryFindChildByName(const std::string &name) const
        {
            const auto foundIt = std::find_if(mChildren.begin(), mChildren.end(), [&name](const auto &child)
                                              { return child->GetName() == name; });
            return foundIt != mChildren.end() ? (*foundIt) : nullptr;
        }

        std::shared_ptr<UiItemBase> UiItemBase::TryFindAncestryUiItem(const std::string &name) const
        {
            std::shared_ptr<UiItemBase> result;
            auto parentWp = mParent;

            while (const auto &parentSp = parentWp.lock())
            {
                const auto &child = parentSp->TryFindChildByName(name);
                if (child)
                {
                    result = std::static_pointer_cast<UiItemBase>(child);
                    break;
                }

                parentWp = parentSp->GetParent();
            }

            return result;
        }

        void UiItemBase::SetAnchor(const eUiAnchor srcAnchor, const eUiAnchor dstAnchor, const std::string &dstUiItemName)
        {
            if (UiAnchorPositionHelper::CheckIsAnchorBindingValid(srcAnchor, dstAnchor))
            {
                if (TryFindAncestryUiItem(dstUiItemName))
                {
                    mAnchors[srcAnchor] = std::make_pair(dstAnchor, dstUiItemName);
                }
                else
                {
                    LogInfo("UiItemBase::SetAnchor => Warning! Try to anchor to ui item which is not parent or sibling");
                }
            }
            else
            {
                LogInfo("UiItemBase::SetAnchor => Warning! Wrong anchor binding. srcAnchor: ", static_cast<uint8_t>(srcAnchor), " dstAnchor: ", static_cast<uint8_t>(dstAnchor));
            }
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

        void UiItemBase::TransformChanged()
        {
            UpdateHierarchyTransform();
            OnTransformChanged();
        }

        void UiItemBase::RebuildTransform()
        {
            if (const auto parentSp = mParent.lock())
            {
                const auto parentWidth = parentSp->GetWidth();
                const auto parentHeight = parentSp->GetHeight();
                const auto parentAbsoluteOrigin = parentSp->GetAbsoluteOrigin();

                if (mAnchors.size())
                {
                    RecalculateAnchorPositions();
                }
                else
                {
                    const auto& translation = glm::clamp(mAbsoluteOrigin, glm::ivec2(), glm::ivec2(parentWidth, parentHeight));
                    mAbsoluteOrigin = translation + parentAbsoluteOrigin;
                }

                RebuildNormalizedTransform(parentSp);
                RebuildTransformMatrix();
            }
        }

        void UiItemBase::RebuildNormalizedTransform(const std::shared_ptr<IUiTransformable> &parent)
        {
            auto rootParentSp = parent->GetRootParent().lock();
            rootParentSp = rootParentSp ? rootParentSp : parent;
            const auto rootWidth = rootParentSp->GetWidth();
            const auto rootHeight = rootParentSp->GetHeight();
            assert(rootWidth != 0 && rootHeight != 0);
            mNormalizedTranslation = glm::vec2(static_cast<float>(mAbsoluteOrigin.x) / static_cast<float>(rootWidth),
                                               static_cast<float>(mAbsoluteOrigin.y) / static_cast<float>(rootHeight));

            mNormalizedScale = glm::vec2(static_cast<float>(mWidth) / static_cast<float>(rootWidth),
                                         static_cast<float>(mHeight) / static_cast<float>(rootHeight));
        }

        void UiItemBase::RebuildTransformMatrix()
        {
            mTransformMatrix = glm::mat4(1);
            const auto &tranlsationToLeftBottomCorner = (glm::vec2(1.0f) - mNormalizedScale);
            const auto translationFromCenter = mNormalizedTranslation - tranlsationToLeftBottomCorner;
            mTransformMatrix *= glm::translate(glm::mat4(1), glm::vec3(translationFromCenter.x, translationFromCenter.y, 0.0f));
            mTransformMatrix *= glm::scale(glm::mat4(1), glm::vec3(mNormalizedScale.x, mNormalizedScale.y, 1.0f));

            SyncDataOnRenderThread();
        }

        void UiItemBase::RebuildBoundingArea()
        {
            const auto halfExtent = glm::ivec2(mWidth / 2, mHeight / 2);
            mBoundingArea = BoundingBox2D(mAbsoluteOrigin + halfExtent, halfExtent);
        }

        void UiItemBase::RecalculateAnchorPositions()
        {
            if (mAnchors.size())
            {
                CalculateHorizontalAnchorPositions();
                CalculateVerticalAnchorPositions();
            }
        }

        void UiItemBase::CalculateHorizontalAnchorPositions()
        {
            if (mAnchors.count(eUiAnchor::HORIZONTAL_CENTER))
            {
                const auto &dstAnchor = mAnchors.at(eUiAnchor::HORIZONTAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.second);
                assert(dstAnchoringUiItem);
                const auto &dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
                mAbsoluteOrigin.x = dstBoundingArea.GetOrigin().x - (mWidth / 2);
            }
            else
            {
            }
        }

        void UiItemBase::CalculateVerticalAnchorPositions()
        {
            if (mAnchors.count(eUiAnchor::VERTICAL_CENTER))
            {
                const auto &dstAnchor = mAnchors.at(eUiAnchor::VERTICAL_CENTER);
                const auto &dstAnchoringUiItem = TryFindAncestryUiItem(dstAnchor.second);
                assert(dstAnchoringUiItem);
                const auto &dstBoundingArea = dstAnchoringUiItem->GetBoundingArea();
                mAbsoluteOrigin.y = dstBoundingArea.GetOrigin().y - (mHeight / 2);
            }
            else
            {
            }
        }

        void UiItemBase::OnTransformChanged()
        {
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
                SyncDataOnRenderThread();
            }
        }

        void UiItemBase::UpdateHierarchyTransform()
        {
            RebuildTransform();
            RebuildBoundingArea();

            for (const auto &child : mChildren)
            {
                child->UpdateHierarchyTransform();
            }
        }

        void UiItemBase::AddUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            RegisterUiItem(uiItem->GetUId(), uiItem->GetName());
            mChildren.emplace_back(uiItem);
            uiItem->OnRegistered();
        }

        void UiItemBase::RemoveUiItem(const std::shared_ptr<UiItemBase> &uiItem)
        {
            UnregisterUiItem(uiItem->GetUId(), uiItem->GetName());
            const auto it = std::remove_if(mChildren.begin(), mChildren.end(), [&](const auto &childUi)
                                           { return childUi->GetUId() == uiItem->GetUId(); });
            mChildren.erase(it);
            uiItem->OnUnregistered();
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

        void UiItemBase::OnRegistered()
        {
        }

        void UiItemBase::OnUnregistered()
        {
        }

        void UiItemBase::Tick(const float deltaTime)
        {
            
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

        void UiItemBase::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiItemBase::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().TryGetSceneRendererWP().lock())
                    {
                        sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, 0, functionId, [this, sceneRenderer, canvasSp]()
                                                       {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                            if (uiSceneProxy)
                            {
                                uiSceneProxy->SetIsVisible(mIsVisible);
                                uiSceneProxy->SetZOrder(mZOrder);
                                uiSceneProxy->SetTransformMatrix(mTransformMatrix);
                            } });
                    }
                }
            }
        }
    }
}