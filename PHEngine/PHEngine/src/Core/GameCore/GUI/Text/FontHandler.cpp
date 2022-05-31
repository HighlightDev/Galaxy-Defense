#include "FontHandler.h"
#include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/GUI/Text/TextMeshCreator.h"
#include "Core/GameCore/GUI/Text/GUIText.h"

using namespace IO;
using namespace Resources;
using namespace EngineUtility;

namespace EngineCore
{
    TextVertexChunkData::TextVertexChunkData()
        : mCurrentChunkOffset(0),
          mTotalChunkSize(0)
    {
    }

    FontRenderData::FontRenderData(const std::shared_ptr<TextMesh> &textMesh,
                                   const std::shared_ptr<ITexture> &fontTextureAtlas,
                                   const std::shared_ptr<FontMetaFile> &fontMetaFile)
        : mPositionChunkData(),
          mTextureCoordinatesChunkData(),
          mVerticesCount(0),
          mTextMesh(textMesh),
          mFontTextureAtlas(fontTextureAtlas),
          mFontMetaFile(fontMetaFile)
    {
    }

    void FontRenderData::RegisterText(const TextFieldProxy &textFieldProxy)
    {
        const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [&](const auto &mProxy)
                                     { return textFieldProxy.mTextFieldId == mProxy.mTextFieldId; });
        assert(it == mTextFields.end());
        mTextFields.emplace_back(textFieldProxy);

        AllocateTextSpace(textFieldProxy);
    }

    void FontRenderData::AllocateTextSpace(const TextFieldProxy &textFieldProxy)
    {
        TextMeshCreator textMeshCreator(*mFontMetaFile.get());
        auto guiText = GUIText(textFieldProxy.mText,
                               textFieldProxy.mFontSize,
                               textFieldProxy.mPosition,
                               textFieldProxy.mLineMaxSize,
                               textFieldProxy.mIsCenteredText);
        auto textMesh = textMeshCreator.CreateTextMesh(guiText);
        const auto &vertexPositions = textMesh.mVertexPositions;
        const auto &textCoordinates = textMesh.mTextureCoords;
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);

        assert(positionVBO && textureCoordinatesVBO);

        const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
        const size_t positionSizeUpdate = vertexPositions.size() * sizeof(float);
        assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
        positionVBO->BindVBO();
        positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
        mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;

        const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
        const size_t texCoordinatesSizeUpdate = textCoordinates.size() * sizeof(float);
        assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
        textureCoordinatesVBO->BindVBO();
        textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
        mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
        textureCoordinatesVBO->UnbindVBO();

        mVerticesCount = mPositionChunkData.mCurrentChunkOffset / positionVBO->GetVectorSize();
    }

    FontHandler::FontHandler()
        : mFontRenderDataMap()
    {
    }

    void FontHandler::RegisterFont(const FontParams &fontParams)
    {
        assert(mFontRenderDataMap.count(fontParams.FontName) == 0);

        const auto &fontMesh = FontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
        const auto &fontTextureAtlas = TexturePool::GetInstance()->GetOrAllocateResource(fontParams.FontTextureAtlas);
        const auto &fontDescriptorFile = std::make_shared<FontMetaFile>(FolderManager::GetInstance()->GetFontsPath() + fontParams.FontDescriptorFile);
        mFontRenderDataMap.emplace(fontParams.FontName, std::make_shared<FontRenderData>(fontMesh, fontTextureAtlas, fontDescriptorFile));

        const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;
        static constexpr size_t positionsPerCharacter = 6;
        static constexpr size_t texCoordinatesPerCharacter = 6;
        const auto &fontRenderData = mFontRenderDataMap.at(fontParams.FontName);
        fontRenderData->mPositionChunkData.mTotalChunkSize = maxFontCharactersCount * positionsPerCharacter * sizeof(float);
        fontRenderData->mTextureCoordinatesChunkData.mTotalChunkSize = maxFontCharactersCount * texCoordinatesPerCharacter * sizeof(float);
    }

    const std::shared_ptr<FontRenderData> &FontHandler::GetFontRenderData(const std::string &fontName) const
    {
        assert(mFontRenderDataMap.count(fontName));
        return mFontRenderDataMap.at(fontName);
    }

    const std::unordered_map<std::string, std::shared_ptr<FontRenderData>>& FontHandler::GetFontRenderDataMap() const
    {
        return mFontRenderDataMap;
    }

    void FontHandler::RegisterText(const TextFieldProxy &textFieldProxy)
    {
        assert(mFontRenderDataMap.count(textFieldProxy.mFontName));
        mFontRenderDataMap.at(textFieldProxy.mFontName)->RegisterText(textFieldProxy);
    }
}