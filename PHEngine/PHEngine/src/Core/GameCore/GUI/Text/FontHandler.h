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

    class FontRenderData
    {
        TextVertexChunkData mPositionChunkData;
        TextVertexChunkData mTextureCoordinatesChunkData;
        TextVertexChunkData mOffsetsChunkData;
        TextVertexChunkData mColorsChunkData;
        size_t mVerticesCount;

        std::shared_ptr<TextMesh> mTextMesh;
        std::shared_ptr<ITexture> mFontTextureAtlas;
        std::shared_ptr<FontMetaFile> mFontMetaFile;

        std::vector<std::shared_ptr<TextFieldProxy>> mTextFields;

    public:
        FontRenderData(const std::shared_ptr<TextMesh> &textMesh,
                       const std::shared_ptr<ITexture> &fontTextureAtlas,
                       const std::shared_ptr<FontMetaFile> &fontMetaFile);

        void RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

        void UnregisterText(const size_t textFieldId);

        TextVertexChunkData &GetPositionChunkDataRef();

        TextVertexChunkData &GetTextureCoordinatesChunkDataRef();

        TextVertexChunkData& GetOffsetChunkDataRef();

        TextVertexChunkData& GetColorChunkDataRef();

        const std::shared_ptr<TextMesh> &GetTextMesh() const;

        const std::shared_ptr<ITexture> &GetFontTextureAtlas() const;

        const std::shared_ptr<FontMetaFile> &GetFontMetaFile() const;

        size_t GetVerticesCount() const;

    private:
        void AllocateTextSpace(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

        void ReallocateTextSpace(const std::shared_ptr<TextFieldProxy> &removeTextFieldProxy);

        void FontBufferSubData(const std::shared_ptr<TextFieldProxy> &textFieldProxy,
                               VertexBufferObjectBase *const positionVBO,
                               VertexBufferObjectBase *const textureCoordinatesVBO,
                               VertexBufferObjectBase *const colorVBO,
                               VertexBufferObjectBase *const offsetVBO);
    };

    class FontHandler
    {
        std::unordered_map<std::string, std::shared_ptr<FontRenderData>> mFontRenderDataMap;

    public:
        FontHandler();

        void RegisterFont(const FontParams &fontParams);

        const std::shared_ptr<FontRenderData> &GetFontRenderData(const std::string &fontName) const;

        const std::unordered_map<std::string, std::shared_ptr<FontRenderData>> &GetFontRenderDataMap() const;

        void RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy);

        void UnregisterText(const std::string &fontName, const size_t textFieldProxyId);
    };
}