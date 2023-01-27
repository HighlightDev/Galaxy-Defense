#include "UiImage.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiImageSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"

#include <functional>

using namespace EngineCore;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore
{
    namespace GUI
    {
        UiImage::UiImage(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent)
            : UiItemBase(canvasParent, parent),
              mTextureSrc(""),
              mTexture(),
              mOpacity(1.0f),
              mRotationDegrees(0.0f),
              mIsFlipped(false)
        {
        }

        UiImage::~UiImage()
        {
            if (mTexture)
            {
                TexturePool::GetInstance()->TryToFreeMemory(mTexture);
            }
        }

        void UiImage::OnRegistered()
        {
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &parentCanvasSp = mParentCanvas.lock())
                {
                    const auto &thisSceneProxy = CreateUiSceneProxy();
                    sceneSp->RegisterUiSceneProxy_OnRenderThread(thisSceneProxy, parentCanvasSp->GetUId());
                }
            }
        }

        void UiImage::OnUnregistered()
        {
        }

        void UiImage::ReallocateTexture()
        {
            const auto &texturePool = TexturePool::GetInstance();
            if (mTexture)
            {
                assert(texturePool->TryToFreeMemory(mTexture));
            }

            mTexture = texturePool->GetOrAllocateResource(mTextureSrc);
        }

        void UiImage::OnPropertiesShouldBeUpdatedOnRenderThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
            SyncDataOnRenderThread();
        }

        void UiImage::SetTextureSrc(const std::string &textureSrc)
        {
            if (mTextureSrc != textureSrc)
            {
                mTextureSrc = textureSrc;
                ReallocateTexture();
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        void UiImage::SetTexture(const std::shared_ptr<ITexture> &texture)
        {
            if (mTexture && mTextureSrc != "")
            {
                assert(TexturePool::GetInstance()->TryToFreeMemory(mTexture));
                mTextureSrc = "";
                mTexture = nullptr;
            }

            mTexture = texture;
            SetIsPropertiesShouldBeUpdated(true);
        }

        std::string UiImage::GetTextureSrc() const
        {
            return mTextureSrc;
        }

        std::shared_ptr<ITexture> UiImage::GetTexture() const
        {
            return mTexture;
        }

        void UiImage::SetOpacity(const float opacity)
        {
            if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
            {
                mOpacity = opacity;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        float UiImage::GetOpacity() const
        {
            return mOpacity;
        }

        void UiImage::SetRotationDegrees(const float rotationDegrees)
        {
            if (!EngineMath::FloatsNearEqual(mRotationDegrees, rotationDegrees))
            {
                mRotationDegrees = rotationDegrees;
                SetIsPropertiesShouldBeUpdated(true);
            }
        }

        float UiImage::GetRotationDegrees() const
        {
            return mRotationDegrees;
        }

        void UiImage::SetIsFlipped(const bool isFlipped)
        {
            if (mIsFlipped != isFlipped)
            {
                mIsFlipped = isFlipped;
            }
        }

        bool UiImage::GetIsFlipped() const
        {
            return mIsFlipped;
        }

        std::shared_ptr<UiSceneProxyBase> UiImage::CreateUiSceneProxy() const
        {
            return std::make_shared<UiImageSceneProxy>(this);
        }

        void UiImage::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiImage::SyncDataOnRenderThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &canvasSp = GetParentCanvas().lock())
                {
                    if (const auto &sceneRenderer = sceneSp->GetThreadManager().GetSceneRendererWP().lock())
                    {
                        const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(GetUId(), canvasSp->GetUId());
                        if (uiSceneProxy)
                        {
                            sceneSp->ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [=]()
                                                           {
                                const auto& imageSceneProxy = std::static_pointer_cast<UiImageSceneProxy>(uiSceneProxy);
                                imageSceneProxy->SetTexture(mTexture);
                                imageSceneProxy->SetOpacity(mOpacity);
                                imageSceneProxy->SetRotationDegrees(mRotationDegrees);
                                imageSceneProxy->SetIsFlipped(mIsFlipped); });
                        }
                        else
                        {
                            mIsPropertiesShouldBeUpdatedOnRenderThread = true;
                        }
                    }
                }
            }
        }
    }
}