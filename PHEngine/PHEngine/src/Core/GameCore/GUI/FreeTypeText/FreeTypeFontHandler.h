#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "FreeTypeFontParams.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

using namespace Graphics;
using namespace Graphics::Texture;

namespace Graphics::OpenGL {
class VertexBufferObjectBase;
}

namespace EngineCore::GUI {

class FreeTypeTextFieldProxy;
class FreeTypeFontAtlas;

struct FreeTypeTextVertexChunkData {
    size_t mCurrentChunkOffset;
    size_t mTotalChunkSize;

    FreeTypeTextVertexChunkData();
};

class FreeTypeFontBatcher {
    FreeTypeTextVertexChunkData mPositionChunkData;
    FreeTypeTextVertexChunkData mTextureCoordinatesChunkData;
    size_t mVerticesCount;

    std::shared_ptr<FreeTypeFontAtlas> mTextFontAtlas;

    std::vector<std::shared_ptr<FreeTypeTextFieldProxy>> mTextFields;

public:
    FreeTypeFontBatcher(const std::shared_ptr<FreeTypeFontAtlas>& fontAtlas);

    void RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

    void UnregisterText(const int32_t textFieldId);

    void TextPositionChanged(const int32_t textFieldId, const glm::vec2& position);

    void TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color);

    void TextChanged(const int32_t textFieldProxyId, const std::string& text);

    void TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible);

    FreeTypeTextVertexChunkData& GetPositionChunkDataRef();

    FreeTypeTextVertexChunkData& GetTextureCoordinatesChunkDataRef();

    const std::shared_ptr<FreeTypeFontAtlas>& GetFreeTypeFontAtlas() const;

    size_t GetVerticesCount() const;

    const std::vector<std::shared_ptr<FreeTypeTextFieldProxy>>& GetFreeTypeTextFieldProxies() const;

    std::shared_ptr<FreeTypeTextFieldProxy> GetFreeTypeTextFieldById(const int32_t textFieldId) const;

    std::shared_ptr<ITexture> GetFontTextureAtlas() const;

private:
    void AllocateTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

    void FreeAllocatedTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& removeTextFieldProxy);

    void ReallocateTextSpace();

    void FontBufferSubData(
        const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy,
        ::Graphics::OpenGL::VertexBufferObjectBase* const positionVBO,
        ::Graphics::OpenGL::VertexBufferObjectBase* const textureCoordinatesVBO);
};

class FreeTypeFontHandler {
    mutable std::unordered_map<FreeTypeFontParams, std::shared_ptr<FreeTypeFontBatcher>> mFontBatcherMap;

public:
    explicit FreeTypeFontHandler();

    void RegisterFont(const FreeTypeFontParams& fontParams) const;

    std::shared_ptr<FreeTypeFontBatcher> GetFontBatcher(const FreeTypeFontParams& fontParams) const;

    const std::unordered_map<FreeTypeFontParams, std::shared_ptr<FreeTypeFontBatcher>>& GetFontBatcherMap() const;

    void RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

    void UnregisterText(const int32_t textFieldProxyId);

    void TextPositionChanged(const int32_t textFieldProxyId, const glm::vec2& position);

    void TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color);

    void TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible);

    void TextChanged(const int32_t textFieldProxyId, const std::string& text);

    void FontSizeChanged(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

    float GetTextNormalizedWidth(const int32_t textFieldProxyId) const;

    float GetTextNormalizedHeight(const int32_t textFieldProxyId) const;

    bool IsTextSubscribedOnSizeChangeUpdate(const int32_t textFieldProxyId) const;

    glm::vec2 GetTextSizeNormalized(const int32_t textFieldProxyId) const;

    glm::ivec2 GetTextScreenSpaceSize(const int32_t textFieldProxyId) const;

    std::shared_ptr<FreeTypeFontBatcher> FindFontBatcherByTextFieldProxyId(const int32_t proxyId) const;
};
} // namespace EngineCore::GUI