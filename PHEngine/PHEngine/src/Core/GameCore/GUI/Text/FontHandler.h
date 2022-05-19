#pragma once

#include "TextMesh.h"
#include "FontParams.h"
#include "FontMetaFile.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <unordered_map>
#include <memory>

using namespace Graphics::Mesh;
using namespace Graphics::Texture;

namespace EngineCore
{
    struct FontRenderData
    {
        std::shared_ptr<TextMesh> mTextMesh;
        std::shared_ptr<ITexture> mFontTextureAtlas;
        std::shared_ptr<FontMetaFile> mFontMetaFile;

        FontRenderData(const std::shared_ptr<TextMesh> &textMesh,
                       const std::shared_ptr<ITexture> &fontTextureAtlas,
                       const std::shared_ptr<FontMetaFile> &fontMetaFile)
            : mTextMesh(textMesh),
              mFontTextureAtlas(fontTextureAtlas),
              mFontMetaFile(fontMetaFile)
        {
        }
    };

    class FontHandler
    {
        std::unordered_map<FontParams, std::shared_ptr<FontRenderData>> mFontMeshMap;

    public:
        FontHandler();

        void RegisterFont(const FontParams &fontParams);

        const std::shared_ptr<FontRenderData> &GetFontRenderData(const FontParams &fontParams) const;
    };
}