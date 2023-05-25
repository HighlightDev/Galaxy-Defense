#pragma once

#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>
#include <string>

namespace EngineCore
{
    namespace GUI
    {
        class UiImage;
    }
}

namespace EngineCore
{
    namespace Scripts
    {
        class UiImageLuaProxy
            : public UiItemBaseLuaProxy
        {
        protected:
            std::string mTextureSrc;

            float mOpacity;

            float mRotationDegrees;

            bool mIsFlipped;

        public:
            explicit UiImageLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiImage> &ownerUiItem);

            void OnLuaThreadDataUpdated(const std::string &jsonParameters) override;

            std::string GetGameThreadData() override;

            void SetTextureSource_FromGameThread(const std::string &textureSrc);

            void SetOpacity_FromGameThread(const float opacity);

            void SetRotationDegrees_FromGameThread(const float rotationDegrees);

            void SetIsFlipped_FromGameThread(const bool isFlipped);

            bool IsAnimationSupported() const override;
        };
    }
}
