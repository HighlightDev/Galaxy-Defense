#include "FontHandler.h"
#include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/GUI/Text/TextMeshCreator.h"
#include "Core/GameCore/GUI/Text/GUIText.h"

#include <algorithm>

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

    void FontRenderData::RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
    {
        const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [&](const auto &mProxy)
                                     { return textFieldProxy->mTextFieldId == mProxy->mTextFieldId; });
        assert(it == mTextFields.end());
        mTextFields.emplace_back(textFieldProxy);

        AllocateTextSpace(textFieldProxy);
    }

    void FontRenderData::UnregisterText(const size_t textFieldId)
    {
        const auto foundIt = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                       { return textFieldId == mProxy->mTextFieldId; });
        auto deleteTextProxy = *foundIt;

        const auto removeIt = std::remove_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                       { return textFieldId == mProxy->mTextFieldId; });
        assert(removeIt != mTextFields.end());
        mTextFields.erase(removeIt);

        ReallocateTextSpace(deleteTextProxy);
    }

    void FontRenderData::AllocateTextSpace(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
    {
        TextMeshCreator textMeshCreator(*mFontMetaFile.get());
        auto guiText = GUIText(textFieldProxy->mText,
                               textFieldProxy->mFontSize,
                               textFieldProxy->mPosition,
                               textFieldProxy->mLineMaxSize,
                               textFieldProxy->mIsCenteredText);
        auto textMesh = textMeshCreator.CreateTextMesh(guiText);
        const auto &vertexPositions = textMesh.mVertexPositions;
        const auto &textCoordinates = textMesh.mTextureCoords;
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);

        assert(positionVBO && textureCoordinatesVBO);

        const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
        const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetVectorElementByteSize();
        assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
        positionVBO->BindVBO();
        positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
        mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
        textFieldProxy->mPositionChunkOffset = positionOffset;
        textFieldProxy->mPositionChunkSize = positionSizeUpdate;

        const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
        const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetVectorElementByteSize();
        assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
        textureCoordinatesVBO->BindVBO();
        textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
        mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
        textFieldProxy->mTextureCoordinatesChunkOffset = texCoordinatesOffset;
        textFieldProxy->mTextureCoordinatesChunkSize = texCoordinatesSizeUpdate;

        textureCoordinatesVBO->UnbindVBO();

        mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetVectorElementByteSize()) / positionVBO->GetVectorSize();
    }

    void FontRenderData::ReallocateTextSpace(const std::shared_ptr<TextFieldProxy> &removeTextFieldProxy)
    {
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        assert(positionVBO && textureCoordinatesVBO);

        if (0 == removeTextFieldProxy->mPositionChunkOffset &&
            0 == removeTextFieldProxy->mTextureCoordinatesChunkOffset) // text that should be removed is at the beginning
        {
            mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
            mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

            for (auto &existingTextProxy : mTextFields)
            {
                TextMeshCreator textMeshCreator(*mFontMetaFile.get());
                auto guiText = GUIText(existingTextProxy->mText,
                                       existingTextProxy->mFontSize,
                                       existingTextProxy->mPosition,
                                       existingTextProxy->mLineMaxSize,
                                       existingTextProxy->mIsCenteredText);
                auto textMesh = textMeshCreator.CreateTextMesh(guiText);
                const auto &vertexPositions = textMesh.mVertexPositions;
                const auto &textCoordinates = textMesh.mTextureCoords;

                const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
                const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetVectorElementByteSize();

                assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
                positionVBO->BindVBO();
                positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
                mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
                existingTextProxy->mPositionChunkOffset = positionOffset;
                existingTextProxy->mPositionChunkSize = positionSizeUpdate;

                const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
                const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetVectorElementByteSize();
                assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
                textureCoordinatesVBO->BindVBO();
                textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
                mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
                existingTextProxy->mTextureCoordinatesChunkOffset = texCoordinatesOffset;
                existingTextProxy->mTextureCoordinatesChunkSize = texCoordinatesSizeUpdate;
            }

            textureCoordinatesVBO->UnbindVBO();
            mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetVectorElementByteSize()) / positionVBO->GetVectorSize();
        }
        else
        {
            std::vector<std::shared_ptr<TextFieldProxy>> textFieldProxiesToReallocate;
            std::copy_if(mTextFields.begin(),
                         mTextFields.end(),
                         std::back_inserter(textFieldProxiesToReallocate),
                         [&](const auto &textField)
                         { return textField->mPositionChunkOffset > removeTextFieldProxy->mPositionChunkOffset; });

            if (textFieldProxiesToReallocate.size()) // if need to reallocate for existing text
            {
                mPositionChunkData.mCurrentChunkOffset = removeTextFieldProxy->mPositionChunkOffset;
                mTextureCoordinatesChunkData.mCurrentChunkOffset = removeTextFieldProxy->mTextureCoordinatesChunkOffset;

                for (auto &textProxy : textFieldProxiesToReallocate)
                {
                    TextMeshCreator textMeshCreator(*mFontMetaFile.get());
                    auto guiText = GUIText(textProxy->mText,
                                           textProxy->mFontSize,
                                           textProxy->mPosition,
                                           textProxy->mLineMaxSize,
                                           textProxy->mIsCenteredText);
                    auto textMesh = textMeshCreator.CreateTextMesh(guiText);
                    const auto &vertexPositions = textMesh.mVertexPositions;
                    const auto &textCoordinates = textMesh.mTextureCoords;

                    const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
                    const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetVectorElementByteSize();

                    assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
                    positionVBO->BindVBO();
                    positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
                    mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
                    textProxy->mPositionChunkOffset = positionOffset;
                    textProxy->mPositionChunkSize = positionSizeUpdate;

                    const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
                    const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetVectorElementByteSize();
                    assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
                    textureCoordinatesVBO->BindVBO();
                    textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
                    mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
                    textProxy->mTextureCoordinatesChunkOffset = texCoordinatesOffset;
                    textProxy->mTextureCoordinatesChunkSize = texCoordinatesSizeUpdate;
                }

                textureCoordinatesVBO->UnbindVBO();
            }
            else
            {
                mPositionChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mPositionChunkSize;
                mTextureCoordinatesChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mTextureCoordinatesChunkSize;
            }

            mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetVectorElementByteSize()) / positionVBO->GetVectorSize();
        }
    }

    const std::shared_ptr<TextMesh> &FontRenderData::GetTextMesh() const
    {
        return mTextMesh;
    }

    const std::shared_ptr<ITexture> &FontRenderData::GetFontTextureAtlas() const
    {
        return mFontTextureAtlas;
    }

    const std::shared_ptr<FontMetaFile> &FontRenderData::GetFontMetaFile() const
    {
        return mFontMetaFile;
    }

    TextVertexChunkData &FontRenderData::GetPositionChunkDataRef()
    {
        return mPositionChunkData;
    }

    TextVertexChunkData &FontRenderData::GetTextureCoordinatesChunkDataRef()
    {
        return mTextureCoordinatesChunkData;
    }

    size_t FontRenderData::GetVerticesCount() const
    {
        return mVerticesCount;
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
        fontRenderData->GetPositionChunkDataRef().mTotalChunkSize = maxFontCharactersCount * positionsPerCharacter * sizeof(float);
        fontRenderData->GetTextureCoordinatesChunkDataRef().mTotalChunkSize = maxFontCharactersCount * texCoordinatesPerCharacter * sizeof(float);
    }

    const std::shared_ptr<FontRenderData> &FontHandler::GetFontRenderData(const std::string &fontName) const
    {
        assert(mFontRenderDataMap.count(fontName));
        return mFontRenderDataMap.at(fontName);
    }

    const std::unordered_map<std::string, std::shared_ptr<FontRenderData>> &FontHandler::GetFontRenderDataMap() const
    {
        return mFontRenderDataMap;
    }

    void FontHandler::RegisterText(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
    {
        assert(mFontRenderDataMap.count(textFieldProxy->mFontName));
        mFontRenderDataMap.at(textFieldProxy->mFontName)->RegisterText(textFieldProxy);
    }

    void FontHandler::UnregisterText(const std::string &fontName, const size_t textFieldProxyId)
    {
        assert(mFontRenderDataMap.count(fontName));
        mFontRenderDataMap.at(fontName)->UnregisterText(textFieldProxyId);
    }
}