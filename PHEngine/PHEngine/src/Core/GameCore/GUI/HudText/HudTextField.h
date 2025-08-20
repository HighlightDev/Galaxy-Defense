#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stdint.h>

#include <memory>
#include <string>

namespace EngineCore {
enum class eTextChangedDataType { VISIBILITY, OFFSET, COLOR, TEXT };

class HudTextField {
    int32_t mTextFieldId;

    bool mIsVisible;

    std::string mFontName;

    std::string mText;

    int32_t mFontSize;

    glm::vec3 mColor;

    glm::vec2 mPosition;

    glm::ivec2 mLineMaxWidthHeight;

    eTextHorizontalAlignmentType mTextHorizontalAlignment;

    eTextVerticalAlignmentType mTextVerticalAlignment;

    glm::vec2 mScreenSpaceSize;

public:
    HudTextField(
        const std::string& fontName,
        const int32_t fontSize,
        const std::string& text,
        const glm::vec3& color,
        const glm::vec2& position,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment);

    HudTextField(
        const std::string& fontName,
        const int32_t fontSize,
        const glm::vec3& color,
        const glm::ivec2& lineMaxWidthHeight,
        const eTextHorizontalAlignmentType textHorizontalAlignment,
        const eTextVerticalAlignmentType textVericalAlignment);

    int32_t GetTextFieldId() const;

    bool GetIsVisible() const;

    std::string GetFontName() const;

    int32_t GetFontSize() const;

    std::string GetText() const;

    glm::vec3 GetColor() const;

    glm::vec2 GetPosition() const;

    void SetText(const std::string& text);

    void SetVisibility(const bool isVisible);

    void SetColor(const glm::vec3& color);

    void SetPosition(const glm::vec2& position);

    glm::ivec2 GetLineMaxWidthHeight() const;

    eTextHorizontalAlignmentType GetTextHorizontalAlignment() const;

    eTextVerticalAlignmentType GetTextVerticalAlignment() const;

    void SetTextScreenSpaceSize(const glm::vec2& screenSpaceSize);

    glm::vec2 GetScreenSpaceSize() const;
};
} // namespace EngineCore