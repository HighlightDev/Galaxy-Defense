#include "FontHandler.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/IoCore/FolderManager.h"

using namespace IO;
using namespace Resources;

namespace EngineCore
{
    FontHandler::FontHandler()
        : mFontMeshMap()
    {
    }

    void FontHandler::RegisterFont(const FontParams &fontParams)
    {
        assert(mFontMeshMap.count(fontParams.FontName) == 0);

        const auto &fontMesh = FontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
        const auto &fontTextureAtlas = TexturePool::GetInstance()->GetOrAllocateResource(fontParams.FontTextureAtlas);
        const auto &fontDescriptorFile = std::make_shared<FontMetaFile>(FolderManager::GetInstance()->GetFontsPath() + fontParams.FontDescriptorFile);
        mFontMeshMap.emplace(fontParams.FontName, std::make_shared<FontRenderData>(fontMesh, fontTextureAtlas, fontDescriptorFile));
    }

    const std::shared_ptr<FontRenderData> &FontHandler::GetFontRenderData(const FontParams &fontParams) const
    {
        assert(mFontMeshMap.count(fontParams.FontName));
        return mFontMeshMap.at(fontParams.FontName);
    }
}