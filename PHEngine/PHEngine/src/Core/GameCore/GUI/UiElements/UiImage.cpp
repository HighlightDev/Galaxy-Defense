#include "UiImage.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiImageSceneProxy.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiImageLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

#include <json/json.hpp>
#include <functional>

using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore
{
    namespace GUI
    {
        UiImage::UiImage()
            : UiItemBase(),
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
                if (const auto &sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                {
                    if (const auto &parentCanvasSp = mParentCanvas.lock())
                    {
                        const auto thisSceneProxy = CreateUiSceneProxy();
                        sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(thisSceneProxy, parentCanvasSp->GetUId());
                    }
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

        void UiImage::OnPropertiesShouldBeUpdatedOnLuaThread()
        {
            UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();

            SyncDataOnLuaThread();
        }

        void UiImage::SetTextureSrc(const std::string &textureSrc)
        {
            if (mTextureSrc != textureSrc)
            {
                mTextureSrc = textureSrc;
                ReallocateTexture();
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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
            SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
            SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
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

        std::shared_ptr<LuaProxy> UiImage::ReplicateLuaProxy()
        {
            return std::make_shared<UiImageLuaProxy>(std::static_pointer_cast<UiImage>(shared_from_this()));
        }

        void UiImage::SyncFromLuaJsonProperties(const std::string &luaJsonPropsStr)
        {
            UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

            const auto &jsonObj = nlohmann::json::parse(luaJsonPropsStr);
            if (jsonObj.contains("texture_source"))
            {
                const auto texture_source = jsonObj["texture_source"].get<std::string>();
                if (mTextureSrc != texture_source)
                {
                    mTextureSrc = texture_source;
                    ReallocateTexture();
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("opacity"))
            {
                const auto opacity = jsonObj["opacity"].get<float>();
                if (!EngineMath::FloatsNearEqual(mOpacity, opacity))
                {
                    mOpacity = opacity;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("rotation_degrees"))
            {
                const auto rotation_degrees = jsonObj["rotation_degrees"].get<float>();
                if (!EngineMath::FloatsNearEqual(mRotationDegrees, rotation_degrees))
                {
                    mRotationDegrees = rotation_degrees;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("is_flipped"))
            {
                const auto is_flipped = jsonObj["is_flipped"].get<bool>();
                if (mIsFlipped != is_flipped)
                {
                    mIsFlipped = is_flipped;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
        }

        void UiImage::SyncDataOnRenderThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiImage::SyncDataOnRenderThread");
            if (mIsSceneProxyReady)
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &canvasSp = GetParentCanvas().lock())
                    {
                        if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                        {
                            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneRenderer, myUId = GetUId(), canasUId = canvasSp->GetUId(), textureSp = mTexture, opacity = mOpacity, rotationDegrees = mRotationDegrees, isFlipped = mIsFlipped]()
                                                                                                {
                                const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canasUId);
                                if (uiSceneProxy)
                                {
                                    const auto& imageSceneProxy = std::static_pointer_cast<UiImageSceneProxy>(uiSceneProxy);
                                    imageSceneProxy->SetTexture(textureSp);
                                    imageSceneProxy->SetOpacity(opacity);
                                    imageSceneProxy->SetRotationDegrees(rotationDegrees);
                                    imageSceneProxy->SetIsFlipped(isFlipped); 
                                } });
                        }
                    }
                }
            }
            else
            {
                mIsPropertiesShouldBeUpdatedOnRenderThread = true;
            }
        }

        void UiImage::SyncDataOnLuaThread()
        {
            static constexpr uint64_t functionId = Hash64_CT("UiImage::SyncDataOnLuaThread");
            if (const auto &sceneSp = GetScene().lock())
            {
                if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                {
                    if (const auto &imageLuaProxy = std::static_pointer_cast<UiImageLuaProxy>(luaScriptProcessorSp->GetLuaProxy(GetLuaProxyId())))
                    {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [imageLuaProxy, opacity = mOpacity, textureSrc = mTextureSrc, rotationDegrees = mRotationDegrees, isFlipped = mIsFlipped]()
                                                                                         {
                            imageLuaProxy->SetOpacity_FromGameThread(opacity);
                            imageLuaProxy->SetTextureSource_FromGameThread(textureSrc);
                            imageLuaProxy->SetRotationDegrees_FromGameThread(rotationDegrees);
                            imageLuaProxy->SetIsFlipped_FromGameThread(isFlipped); });
                    }
                }
            }
        }
    }
}