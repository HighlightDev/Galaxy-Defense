// #pragma once

// #include "Core/CommonCore/Assertion.h"
// #include "Core/GraphicsCore/Texture/ITexture.h"
// #include "FreeTypeFontAtlas.h"
// #include "FreeTypeTextFieldProxy.h"

// #include <memory>
// #include <unordered_map>
// #include <vector>

// using namespace Graphics;
// using namespace Graphics::Texture;

// namespace EngineCore {
// struct FreeTypeTextVertexChunkData {
//     size_t mCurrentChunkOffset;
//     size_t mTotalChunkSize;

//     FreeTypeTextVertexChunkData();
// };

// class FreeTypeFontBatcher {
//     FreeTypeTextVertexChunkData mPositionChunkData;
//     FreeTypeTextVertexChunkData mTextureCoordinatesChunkData;
//     size_t mVerticesCount;

//     std::shared_ptr<FreeTypeFontAtlas> mTextFontAtlas;

//     std::vector<std::shared_ptr<FreeTypeTextFieldProxy>> mTextFields;

// public:
//     FreeTypeFontBatcher(const std::shared_ptr<FreeTypeFontAtlas>& fontAtlas);

//     void RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

//     void UnregisterText(const int32_t textFieldId);

//     void TextPositionChanged(const int32_t textFieldId, const glm::vec2& position);

//     void TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color);

//     void TextChanged(const int32_t textFieldProxyId, const std::string& text);

//     void TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible);

//     TextVertexChunkData& GetPositionChunkDataRef();

//     TextVertexChunkData& GetTextureCoordinatesChunkDataRef();

//     const std::shared_ptr<FreeTypeFontAtlas>& GetFreeTypeFontAtlas() const;

//     size_t GetVerticesCount() const;

//     const std::vector<std::shared_ptr<FreeTypeTextFieldProxy>>& GetFreeTypeTexFieldProxies() const;

//     const std::shared_ptr<FreeTypeTextFieldProxy>& GetFreeTypeTextFieldById(const int32_t textFieldId) const;

// private:
//     void AllocateTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

//     void FreeAllocatedTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& removeTextFieldProxy);

//     void ReallocateTextSpace();

//     void FontBufferSubData(
//         const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy,
//         VertexBufferObjectBase* const positionVBO,
//         VertexBufferObjectBase* const textureCoordinatesVBO);
// };

// class FreeTypeFontHandler {
//     std::unordered_map<std::string, std::shared_ptr<FreeTypeFontBatcher>> mFontBatcherMap;

// public:
//     FreeTypeFontHandler();

//     void RegisterFont(const std::string& fontFileName);

//     std::shared_ptr<FreeTypeFontBatcher> GetFontBatcher(const std::string& fontName) const;

//     const std::unordered_map<std::string, std::shared_ptr<FreeTypeFontBatcher>>& GetFontBatcher() const;

//     void RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy);

//     void UnregisterText(const std::string& fontName, const int32_t textFieldProxyId);

//     void TextPositionChanged(const std::string& fontName, const int32_t textFieldProxyId, const glm::vec2& position);

//     void TextColorChanged(const std::string& fontName, const int32_t textFieldProxyId, const glm::vec3& color);

//     void TextVisibilityChanged(const std::string& fontName, const int32_t textFieldProxyId, const bool bIsVisible);

//     void TextChanged(const std::string& fontName, const int32_t textFieldProxyId, const std::string& text);

//     float GetTextWidth(const std::string& fontName, const int32_t textFieldProxyId) const;

//     float GetTextHeight(const std::string& fontName, const int32_t textFieldProxyId) const;

//     bool IsTextSubscribedOnSizeChangeUpdate(const std::string& fontName, const int32_t textFieldProxyId) const;

//     glm::ivec2 GetTextScreenSpaceSize(const std::string& fontFileName, const int32_t textFieldProxyId) const;
// };
// } // namespace EngineCore