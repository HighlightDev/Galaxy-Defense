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

    struct FontRenderData
    {
        TextVertexChunkData mPositionChunkData;
        TextVertexChunkData mTextureCoordinatesChunkData;
        size_t mVerticesCount;

        std::shared_ptr<TextMesh> mTextMesh;
        std::shared_ptr<ITexture> mFontTextureAtlas;
        std::shared_ptr<FontMetaFile> mFontMetaFile;

        std::vector<TextFieldProxy> mTextFields;

        FontRenderData(const std::shared_ptr<TextMesh> &textMesh,
                       const std::shared_ptr<ITexture> &fontTextureAtlas,
                       const std::shared_ptr<FontMetaFile> &fontMetaFile);

        void RegisterText(const TextFieldProxy &textFieldProxy);

        void AllocateTextSpace(const TextFieldProxy &textFieldProxy);
    };

    class FontHandler
    {
        std::unordered_map<std::string, std::shared_ptr<FontRenderData>> mFontRenderDataMap;

    public:
        FontHandler();

        void RegisterFont(const FontParams &fontParams);

        const std::shared_ptr<FontRenderData> &GetFontRenderData(const std::string &fontName) const;

        const std::unordered_map<std::string, std::shared_ptr<FontRenderData>>& GetFontRenderDataMap() const;

        void RegisterText(const TextFieldProxy &textFieldProxy);
    };
}