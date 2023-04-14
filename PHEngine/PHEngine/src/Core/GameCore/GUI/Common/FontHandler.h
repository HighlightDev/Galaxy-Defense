#pragma once

#include "TextMesh.h"
#include "FontParams.h"
#include "FontMetaFile.h"
#include "TextFieldProxy.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/CommonCore/Assertion.h"

#include <unordered_map>
#include <memory>
#include <vector>

using namespace Graphics;
using namespace Graphics::Mesh;
using namespace Graphics::Texture;

namespace EngineCore
{
    struct TextVertexChunkData
    {
        size_t mCurrentChunkOffset;
        size_t mTotalChunkSize;

        TextVertexChunkData();
    };

    class FontBatcher
    {
        TextVertexChunkData mPositionChunkData;
        TextVertexChunkData mTextureCoordinatesChunkData;
        size_t mVerticesCount;

        std::shared_ptr<TextMesh> mTextMesh;
        std::shared_ptr<ITexture> mFontTextureAtlas;
        std::shared_ptr<FontMetaFile> mFontMetaFile;

        std::vector<std::shared_ptr<TextFieldProxy>> mTextFields;

    public:
        FontBatcher(const std::shared_ptr<TextMesh> &textMesh,
                       const std::shared_ptr<ITexture> &fontTextureAtlas,
                       const std::shared_ptr<FontMetaFile> &fontMetaFile);

        void RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

        void UnregisterText(const int32_t textFieldId);

        void TextPositionChanged(const int32_t textFieldId, const glm::vec2& position);

        void TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color);

        void TextChanged(const int32_t textFieldProxyId, const std::string& text);

        void TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible);

        TextVertexChunkData &GetPositionChunkDataRef();

        TextVertexChunkData &GetTextureCoordinatesChunkDataRef();

        const std::shared_ptr<TextMesh> &GetTextMesh() const;

        const std::shared_ptr<ITexture> &GetFontTextureAtlas() const;

        const std::shared_ptr<FontMetaFile> &GetFontMetaFile() const;

        size_t GetVerticesCount() const;

        const std::vector<std::shared_ptr<TextFieldProxy>>& GetTexFieldProxies() const;

        const std::shared_ptr<TextFieldProxy>& GetTextFieldById(const int32_t textFieldId) const;

    private:
        void AllocateTextSpace(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

        void FreeAllocatedTextSpace(const std::shared_ptr<TextFieldProxy> &removeTextFieldProxy);

        void ReallocateTextSpace();

        void FontBufferSubData(const std::shared_ptr<TextFieldProxy> &textFieldProxy,
                               VertexBufferObjectBase *const positionVBO,
                               VertexBufferObjectBase *const textureCoordinatesVBO);
    };

    class FontHandler
    {
        std::unordered_map<std::string, std::shared_ptr<FontBatcher>> mFontBatcher;

    public:
        FontHandler();

        void RegisterFont(const FontParams &fontParams);

        const std::shared_ptr<FontBatcher> &GetFontBatcher(const std::string &fontName) const;

        const std::unordered_map<std::string, std::shared_ptr<FontBatcher>> &GetFontBatcher() const;

        void RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

        void UnregisterText(const std::string &fontName, const int32_t textFieldProxyId);

        void TextPositionChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec2 &position);

        void TextColorChanged(const std::string& fontName, const int32_t textFieldProxyId, const glm::vec3& color);

        void TextVisibilityChanged(const std::string& fontName, const int32_t textFieldProxyId, const bool bIsVisible);

        void TextChanged(const std::string& fontName, const int32_t textFieldProxyId, const std::string& text);

        float GetTextWidth(const std::string& fontName, const int32_t textFieldProxyId) const;

        float GetTextHeight(const std::string& fontName, const int32_t textFieldProxyId) const;

        bool IsTextSubscribedOnSizeChangeUpdate(const std::string& fontName, const int32_t textFieldProxyId) const;

        glm::vec2 GetTextScreenSpaceSize(const std::string& fontName, const int32_t textFieldProxyId) const;
    };
}