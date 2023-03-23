#pragma once

#include "UiItemBase.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"

#include <glm/vec3.hpp>

namespace Graphics
{
    namespace Proxy
    {
        class UiSceneProxyBase;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class LuaProxy;
    }
}

using namespace EngineCore;

namespace EngineCore
{
    class UiCanvas;

    namespace GUI
    {
        class UiLabel : public UiItemBase
        {
            std::string mText;

            float mOpacity;

            const std::string mFontName;

            float mFontSize;

            float mTextLineWidth;

            glm::vec3 mTextColor;

            eTextHorizontalAlignmentType mTextHorizontalAlignment{eTextHorizontalAlignmentType::LEFT};

        public:
            explicit UiLabel(const std::weak_ptr<UiCanvas> &canvasParent, const std::weak_ptr<IUiTransformable> &parent, const std::string &fontName);

            ~UiLabel() override;

            void SetText(const std::string &text);

            std::string GetText() const;

            void SetOpacity(const float opacity);

            float GetOpacity() const;

            std::string GetFontName() const;

            float GetTextLineWidth() const;

            void SetFontSize(const float fontSize);

            float GetFontSize() const;

            void SetTextColor(const glm::vec3& color);

            void SetTextColor(const uint32_t hexColor);

            glm::vec3 GetTextColor() const;

            void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

            eTextHorizontalAlignmentType GetTextHorizontalAlignment() const;
            
            std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

            std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;
            
            void OnPropertiesShouldBeUpdatedOnRenderThread() override;

        protected:
            void OnRegistered() override;

            void OnUnregistered() override;

        private:
            void SyncDataOnRenderThread();
        };
    }
}