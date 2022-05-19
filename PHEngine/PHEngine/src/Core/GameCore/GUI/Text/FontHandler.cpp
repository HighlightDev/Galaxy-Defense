#include "FontHandler.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

using namespace Resources;

namespace EngineCore
{
    FontHandler::FontHandler()
        : mFontMeshMap()
    {
    }

    void FontHandler::RegisterFont(const FontParams &fontParams)
    {
        assert(mFontMeshMap.count(fontParams) == 0);

        const auto &fontMesh = FontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
        const auto &fontTextureAtlas = TexturePool::GetInstance()->GetOrAllocateResource(fontParams.FontTextureAtlas);
        const auto &fontDescriptorFile = std::make_shared<FontMetaFile>(fontParams.FontDescriptorFile);
        mFontMeshMap.emplace(fontParams, std::make_shared<FontRenderData>(fontMesh, fontTextureAtlas, fontDescriptorFile));
    }

    const std::shared_ptr<FontRenderData> &FontHandler::GetFontRenderData(const FontParams &fontParams) const
    {
        assert(mFontMeshMap.count(fontParams));
        return mFontMeshMap.at(fontParams);
    }
}