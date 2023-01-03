#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <stdint.h>
#include <string>
#include <memory>

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"

namespace EngineCore
{
    class HudTextField
        : public std::enable_shared_from_this<HudTextField>
    {
        friend class UiComponent;

        int32_t mTextFieldId;

        bool mIsVisible;

        std::string mFontName;

        std::string mText;

        float mFontSize;

        glm::vec3 mColor;

        glm::vec2 mPosition;

        float mLineMaxWidth;

        int32_t mNumberOfLines;

        eTextHorizontalAlignmentType mTextHorizontalAlignment;

        bool mIsRegistered;

        glm::vec2 mScreenSpaceSize;

    public:
        HudTextField(const std::string &fontName,
                  const float fontSize,
                  const std::string &text,
                  const glm::vec3 &color,
                  const glm::vec2 &position,
                  const float lineMaxSize,
                  const int32_t numberOfLines,
                  const eTextHorizontalAlignmentType textHorizontalAlignment);

        HudTextField(const std::string &fontName,
                  const float fontSize,
                  const glm::vec3 &color,
                  const float lineMaxSize,
                  const int32_t numberOfLines,
                  const eTextHorizontalAlignmentType textHorizontalAlignment);

        std::shared_ptr<HudTextField> GetSharedFromThis() const;

        int32_t GetTextFieldId() const;

        bool GetIsVisible() const;

        std::string GetFontName() const;

        float GetFontSize() const;

        std::string GetText() const;

        glm::vec3 GetColor() const;

        glm::vec2 GetPosition() const;

        void SetText(const std::string &text);

        void SetVisibility(const bool isVisible);

        void SetColor(const glm::vec3 &color);

        void SetPosition(const glm::vec2 &position);

        float GetLineMaxSize() const;

        int32_t GetNumberOfLines() const;

        eTextHorizontalAlignmentType GetTextHorizontalAlignment() const;

        bool GetIsRegistered() const;

        void SetTextScreenSpaceSize(const glm::vec2& screenSpaceSize);

        glm::vec2 GetScreenSpaceSize() const;

    private:
        void RegisterText(const bool receiveUpdateOnTextScreenSpaceSizeChanged);

        void UnregisterText();
    };
}