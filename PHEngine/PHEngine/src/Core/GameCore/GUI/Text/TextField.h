#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <stdint.h>
#include <string>
#include <memory>

namespace EngineCore
{
    class TextField
        : public std::enable_shared_from_this<TextField>
    {
        friend class UiComponent;

        int32_t mTextFieldId;

        static size_t s_TotalTextFieldId;

        bool mIsVisible;

        std::string mFontName;

        std::string mText;

        float mFontSize;

        glm::vec3 mColor;

        glm::vec2 mPosition;

        float mLineMaxSize;

        int32_t mNumberOfLines;

        bool mIsCenteredText;

        bool mIsRegistered;

        glm::vec2 mScreenSpaceSize;

    public:
        TextField(const std::string &fontName,
                  const float fontSize,
                  const std::string &text,
                  const glm::vec3 &color,
                  const glm::vec2 &position,
                  const float lineMaxSize,
                  const int32_t numberOfLines,
                  const bool isCenteredText);

        TextField(const std::string &fontName,
                  const float fontSize,
                  const glm::vec3 &color,
                  const float lineMaxSize,
                  const int32_t numberOfLines,
                  const bool isCenteredText);

        std::shared_ptr<TextField> GetSharedFromThis() const;

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

        bool GetIsCentered() const;

        bool GetIsRegistered() const;

        void SetTextScreenSpaceSize(const glm::vec2& screenSpaceSize);

        glm::vec2 GetScreenSpaceSize() const;

    private:
        void RegisterText(const bool receiveUpdateOnTextScreenSpaceSizeChanged);

        void UnregisterText();
    };
}