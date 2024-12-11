#include "UiImage.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/UiSceneProxy/UiImageSceneProxy.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"
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
        UiImage::UiImage(const std::string &name)
            : UiItemBase(name),
              mTextureSrc(""),
              mTexture(),
              mColor(),
              mIsCustomColor(false),
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
                        sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(std::static_pointer_cast<UiImage>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
                    }
                }
            }
        }

        void UiImage::OnUnregistered()
        {
        }

        void UiImage::ReallocateTexture(const bool updateRenderThreadData, const bool updateLuaThreadData)
        {
            static constexpr uint64_t functionId = Hash64_CT("UiImage::ReallocateTexture");
            if (const auto &sceneSp = GetScene().lock())
            {
                auto &interThreadMngr = sceneSp->GetInterThreadCommunicationManager();
                interThreadMngr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId,
                    [this, updateRenderThreadData, updateLuaThreadData]() {
                    const auto &texturePool = TexturePool::GetInstance();
                    if (mTexture) {
                        assert(texturePool->TryToFreeMemory(mTexture));
                    }
                    mTexture = texturePool->GetOrAllocateResource(mTextureSrc);
                    
                    if (updateRenderThreadData) {
                        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                    }
                    if (updateLuaThreadData) {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
                    }
                });
            }
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
                ReallocateTexture(true, true);
            }
        }

        void UiImage::SetTexture(const std::shared_ptr<ITexture> &texture)
        {
            static constexpr uint64_t functionId = Hash64_CT("UiImage::SetTexture");
            if (const auto &sceneSp = GetScene().lock())
            {
                auto &interThreadMngr = sceneSp->GetInterThreadCommunicationManager();
                interThreadMngr.ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [this, texture]() {
                    if (mTexture && mTextureSrc != "")
                    {
                        assert(TexturePool::GetInstance()->TryToFreeMemory(mTexture));
                        mTextureSrc = "";
                        mTexture = nullptr;
                    }

                    mTexture = texture;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                    SetIsPropertiesShouldBeUpdatedOnLuaThread(true); 
                });
            }
        }

        std::string UiImage::GetTextureSrc() const
        {
            return mTextureSrc;
        }

        std::shared_ptr<ITexture> UiImage::GetTexture() const
        {
            return mTexture;
        }

        void UiImage::SetTextureColor(const glm::vec3 &color)
        {
            if (!EngineMath::CheckSimilarityVec3(mColor, color))
            {
                mColor = color;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        void UiImage::SetTextureColor(const uint8_t r, const uint8_t g, const uint8_t b)
        {
            static constexpr float INV_COLOR_MAX_BYTE_VALUE = 1.0f / 255.0f;
            glm::vec3 color = glm::vec3(static_cast<float>(r) * INV_COLOR_MAX_BYTE_VALUE,
                                        static_cast<float>(g) * INV_COLOR_MAX_BYTE_VALUE,
                                        static_cast<float>(b) * INV_COLOR_MAX_BYTE_VALUE);
            SetTextureColor(color);
        }

        glm::vec3 UiImage::GetTextureColor() const
        {
            return mColor;
        }

        void UiImage::SetIsCustomColorEnabled(const bool isCustomColorEnabled)
        {
            if (mIsCustomColor != isCustomColorEnabled)
            {
                mIsCustomColor = isCustomColorEnabled;
                SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
            }
        }

        bool UiImage::IsCustomColorEnabled() const
        {
            return mIsCustomColor;
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

        std::string UiImage::GetUiTypeString() const
        {
            return "UiImage";
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
                    ReallocateTexture(true, false);
                }
            }
            if (jsonObj.contains("is_custom_color"))
            {
                const auto isCustomColor = jsonObj["is_custom_color"].get<bool>();
                if (mIsCustomColor != isCustomColor)
                {
                    mIsCustomColor = isCustomColor;
                    SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
                }
            }
            if (jsonObj.contains("color"))
            {
                const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["color"]);
                if (!EngineMath::CheckSimilarityVec3(color, mColor))
                {
                    mColor = color;
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
            if (mIsSceneProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &canvasSp = GetParentCanvas().lock())
                    {
                        if (const auto &sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock())
                        {
                            sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [sceneRenderer, myUId = GetUId(), canasUId = canvasSp->GetUId(), textureSp = mTexture, textureColor = mColor, isCustomColor = mIsCustomColor, opacity = mOpacity, rotationDegrees = mRotationDegrees, isFlipped = mIsFlipped]()
                                                                                                {
                                const auto &uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canasUId);
                                if (uiSceneProxy)
                                {
                                    const auto& imageSceneProxy = std::static_pointer_cast<UiImageSceneProxy>(uiSceneProxy);
                                    imageSceneProxy->SetTexture(textureSp);
                                    imageSceneProxy->SetColor(textureColor);
                                    imageSceneProxy->SetUseCustomColor(isCustomColor);
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
            if (mIsLuaProxyReady.load(std::memory_order::memory_order_seq_cst))
            {
                if (const auto &sceneSp = GetScene().lock())
                {
                    if (const auto &luaScriptProcessorSp = GetLuaScriptProcessorWp().lock())
                    {
                        SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, GetUId(), functionId, [luaScriptProcessorSp, luaProxyId = GetLuaProxyId(), textureColor = mColor, isCustomColor = mIsCustomColor, opacity = mOpacity, textureSrc = mTextureSrc, rotationDegrees = mRotationDegrees, isFlipped = mIsFlipped]()
                                                                                         {
                        if (const auto &imageLuaProxy = std::static_pointer_cast<UiImageLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId)))
                        {
                            imageLuaProxy->SetOpacity_FromGameThread(opacity);
                            imageLuaProxy->SetColor_FromGameThread(textureColor);
                            imageLuaProxy->SetUseCustomColor_FromGameThread(isCustomColor);
                            imageLuaProxy->SetTextureSource_FromGameThread(textureSrc);
                            imageLuaProxy->SetRotationDegrees_FromGameThread(rotationDegrees);
                            imageLuaProxy->SetIsFlipped_FromGameThread(isFlipped); 
                        } });
                    }
                }
            }
        }
    }
}