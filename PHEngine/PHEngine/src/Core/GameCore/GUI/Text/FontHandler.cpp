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
          mOffsetsChunkData(),
          mColorsChunkData(),
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

    void FontRenderData::FontBufferSubData(const std::shared_ptr<TextFieldProxy> &textFieldProxy,
                                           VertexBufferObjectBase *const positionVBO,
                                           VertexBufferObjectBase *const textureCoordinatesVBO,
                                           VertexBufferObjectBase *const colorVBO,
                                           VertexBufferObjectBase *const offsetVBO)
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

        const size_t offsetsCount = vertexPositions.size();
        std::vector<float> offsets(offsetsCount);
        for (size_t offsetIndex = 0; offsetIndex < offsetsCount; offsetIndex += 2)
        {
            offsets[offsetIndex] = textFieldProxy->mPosition.x;
            offsets[offsetIndex + 1] = textFieldProxy->mPosition.y;
        }

        const size_t colorsCount = (vertexPositions.size() / positionVBO->GetVectorSize()) * colorVBO->GetVectorSize();
        std::vector<float> colors(colorsCount);
        for (size_t colorIndex = 0; colorIndex < colorsCount; colorIndex += 3)
        {
            colors[colorIndex] = textFieldProxy->mColor.r;
            colors[colorIndex + 1] = textFieldProxy->mColor.g;
            colors[colorIndex + 2] = textFieldProxy->mColor.b;
        }

        // positions
        const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
        const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetElementByteSize();
        assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
        positionVBO->BindVBO();
        positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
        mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
        textFieldProxy->mPositionChunkOffset = positionOffset;
        textFieldProxy->mPositionChunkSize = positionSizeUpdate;

        // texture coordinates
        const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
        const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetElementByteSize();
        assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
        textureCoordinatesVBO->BindVBO();
        textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
        mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
        textFieldProxy->mTextureCoordinatesChunkOffset = texCoordinatesOffset;
        textFieldProxy->mTextureCoordinatesChunkSize = texCoordinatesSizeUpdate;

        // offsets
        const size_t offsetChunkStart = mOffsetsChunkData.mCurrentChunkOffset;
        const size_t offsetSizeUpdate = offsets.size() * offsetVBO->GetElementByteSize();
        assert(offsetChunkStart + offsetSizeUpdate <= mOffsetsChunkData.mTotalChunkSize);
        offsetVBO->BindVBO();
        offsetVBO->BufferSubData(offsetChunkStart, offsetSizeUpdate, offsets.data());
        mOffsetsChunkData.mCurrentChunkOffset = offsetChunkStart + offsetSizeUpdate;
        textFieldProxy->mOffsetChunkOffset = offsetChunkStart;
        textFieldProxy->mOffsetChunkSize = offsetSizeUpdate;

        // colors
        const size_t colorChunkStart = mColorsChunkData.mCurrentChunkOffset;
        const size_t colorSizeUpdate = colors.size() * colorVBO->GetElementByteSize();
        assert(colorChunkStart + colorSizeUpdate <= mColorsChunkData.mTotalChunkSize);
        colorVBO->BindVBO();
        colorVBO->BufferSubData(colorChunkStart, colorSizeUpdate, colors.data());
        mColorsChunkData.mCurrentChunkOffset = colorChunkStart + colorSizeUpdate;
        textFieldProxy->mColorChunkOffset = colorChunkStart;
        textFieldProxy->mColorChunkSize = colorSizeUpdate;
    }

    void FontRenderData::AllocateTextSpace(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
    {
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        auto *const offsetVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::CUSTOM_0);
        auto *const colorVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::COLOR);
        assert(positionVBO && textureCoordinatesVBO && offsetVBO && colorVBO);

        FontBufferSubData(textFieldProxy, positionVBO, textureCoordinatesVBO, colorVBO, offsetVBO);
        textureCoordinatesVBO->UnbindVBO();

        mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
    }

    void FontRenderData::ReallocateTextSpace(const std::shared_ptr<TextFieldProxy> &removeTextFieldProxy)
    {
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        auto *const offsetVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::CUSTOM_0);
        auto *const colorVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::COLOR);
        assert(positionVBO && textureCoordinatesVBO && offsetVBO && colorVBO);

        if (0 == removeTextFieldProxy->mPositionChunkOffset &&
            0 == removeTextFieldProxy->mTextureCoordinatesChunkOffset &&
            0 == removeTextFieldProxy->mOffsetChunkOffset &&
            0 == removeTextFieldProxy->mColorChunkOffset) // text that should be removed is at the beginning
        {
            mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
            mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;
            mOffsetsChunkData.mCurrentChunkOffset = 0;
            mColorsChunkData.mCurrentChunkOffset = 0;

            for (auto &textProxy : mTextFields)
            {
                FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO, colorVBO, offsetVBO);
            }
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
                mOffsetsChunkData.mCurrentChunkOffset = removeTextFieldProxy->mOffsetChunkOffset;
                mColorsChunkData.mCurrentChunkOffset = removeTextFieldProxy->mColorChunkOffset;

                for (auto &textProxy : textFieldProxiesToReallocate)
                {
                    FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO, colorVBO, offsetVBO);
                }
            }
            else
            {
                mPositionChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mPositionChunkSize;
                mTextureCoordinatesChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mTextureCoordinatesChunkSize;
                mOffsetsChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mOffsetChunkSize;
                mColorsChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mColorChunkSize;
            }
        }

        textureCoordinatesVBO->UnbindVBO();
        mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
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

    TextVertexChunkData &FontRenderData::GetOffsetChunkDataRef()
    {
        return mOffsetsChunkData;
    }

    TextVertexChunkData &FontRenderData::GetColorChunkDataRef()
    {
        return mColorsChunkData;
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
        static constexpr size_t verticesPerCharacter = 6;
        const auto &fontRenderData = mFontRenderDataMap.at(fontParams.FontName);

        auto *const positionVBO = fontMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = fontMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        auto *const offsetVBO = fontMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::CUSTOM_0);
        auto *const colorVBO = fontMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::COLOR);
        assert(positionVBO && textureCoordinatesVBO && offsetVBO && colorVBO);

        fontRenderData->GetPositionChunkDataRef().mTotalChunkSize =
            maxFontCharactersCount *
            verticesPerCharacter *
            positionVBO->GetVectorSize() *
            positionVBO->GetElementByteSize();

        fontRenderData->GetTextureCoordinatesChunkDataRef().mTotalChunkSize =
            maxFontCharactersCount *
            verticesPerCharacter *
            textureCoordinatesVBO->GetVectorSize() *
            textureCoordinatesVBO->GetElementByteSize();

        fontRenderData->GetOffsetChunkDataRef().mTotalChunkSize =
            maxFontCharactersCount *
            verticesPerCharacter *
            offsetVBO->GetVectorSize() *
            offsetVBO->GetElementByteSize();

        fontRenderData->GetColorChunkDataRef().mTotalChunkSize =
            maxFontCharactersCount *
            verticesPerCharacter *
            colorVBO->GetVectorSize() *
            colorVBO->GetElementByteSize();
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