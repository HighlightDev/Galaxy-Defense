#include "FontHandler.h"
#include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/GameCore/GUI/Text/TextMeshCreator.h"

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

        if (textFieldProxy->mText != "") // if text is empty - skip allocation
        {
            AllocateTextSpace(textFieldProxy);
        }
    }

    void FontRenderData::UnregisterText(const int32_t textFieldId)
    {
        const auto foundIt = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                          { return textFieldId == mProxy->mTextFieldId; });
        assert(foundIt != mTextFields.end());

        auto deleteTextProxy = *foundIt;
        const auto removeIt = std::remove_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                             { return textFieldId == mProxy->mTextFieldId; });

        mTextFields.erase(removeIt);

        FreeAllocatedTextSpace(deleteTextProxy);
    }

    void FontRenderData::TextPositionChanged(const int32_t textFieldId, const glm::vec2 &position)
    {
        const auto foundIt = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                          { return textFieldId == mProxy->mTextFieldId; });
        if (foundIt != mTextFields.end())
        {
            (*foundIt)->mPosition = position;
        }
    }

    void FontRenderData::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3 &color)
    {
        const auto foundIt = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                          { return textFieldProxyId == mProxy->mTextFieldId; });
        if (foundIt != mTextFields.end())
        {
            (*foundIt)->mColor = color;
        }
    }

    void FontRenderData::TextChanged(const int32_t textFieldProxyId, const std::string &text)
    {
        const auto foundIt = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                          { return textFieldProxyId == mProxy->mTextFieldId; });
        if (foundIt != mTextFields.end())
        {
            (*foundIt)->mText = text;
        }

        ReallocateTextSpace();
    }

    void FontRenderData::TextVisibilityChanged(const int32_t textFieldProxyId, const bool isVisible)
    {
        const auto foundIt = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto &mProxy)
                                          { return textFieldProxyId == mProxy->mTextFieldId; });
        if (foundIt != mTextFields.end() && (*foundIt)->mText != "")
        {
            (*foundIt)->mIsVisible = isVisible;
        }
    }

    void FontRenderData::FontBufferSubData(const std::shared_ptr<TextFieldProxy> &textFieldProxy,
                                           VertexBufferObjectBase *const positionVBO,
                                           VertexBufferObjectBase *const textureCoordinatesVBO)
    {
        TextMeshCreator textMeshCreator(mFontMetaFile);
        auto textMesh = textMeshCreator.CreateTextMesh(textFieldProxy);
        const auto &vertexPositions = textMesh.mVertexPositions;
        const auto &textCoordinates = textMesh.mTextureCoords;

        // positions
        const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
        textFieldProxy->mVertexStart = positionOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize());
        const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetElementByteSize();
        assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
        positionVBO->BindVBO();
        positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
        mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
        textFieldProxy->mPositionChunkOffset = positionOffset;
        textFieldProxy->mPositionChunkSize = positionSizeUpdate;
        textFieldProxy->mVerticesCount = mPositionChunkData.mCurrentChunkOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize()) - textFieldProxy->mVertexStart;

        // texture coordinates
        const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
        const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetElementByteSize();
        assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
        textureCoordinatesVBO->BindVBO();
        textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
        mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
        textFieldProxy->mTextureCoordinatesChunkOffset = texCoordinatesOffset;
        textFieldProxy->mTextureCoordinatesChunkSize = texCoordinatesSizeUpdate;
        textFieldProxy->mCreatedMeshTextWidth = textMesh.mTextWidth;
        textFieldProxy->mCreatedMeshTextHeight = textMesh.mTextHeight;
    }

    void FontRenderData::AllocateTextSpace(const std::shared_ptr<TextFieldProxy> &textFieldProxy)
    {
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        assert(positionVBO && textureCoordinatesVBO);

        FontBufferSubData(textFieldProxy, positionVBO, textureCoordinatesVBO);
        textureCoordinatesVBO->UnbindVBO();

        mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
    }

    void FontRenderData::ReallocateTextSpace()
    {
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        assert(positionVBO && textureCoordinatesVBO);

        mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
        mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

        for (auto &textProxy : mTextFields)
        {
            if (textProxy->mText != "") // if text is empty - skip allocation
            {
                FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
            }
        }
    }

    void FontRenderData::FreeAllocatedTextSpace(const std::shared_ptr<TextFieldProxy> &removeTextFieldProxy)
    {
        auto *const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::POSITION);
        auto *const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName(eAttribArrayIndexName::TEXTURE_COORDINATES);
        assert(positionVBO && textureCoordinatesVBO);

        if (0 == removeTextFieldProxy->mPositionChunkOffset) // text that should be removed is at the beginning
        {
            mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
            mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

            for (auto &textProxy : mTextFields)
            {
                FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
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
                for (auto &textProxy : textFieldProxiesToReallocate)
                {
                    FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
                }
            }
            else
            {
                mPositionChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mPositionChunkSize;
                mTextureCoordinatesChunkData.mCurrentChunkOffset -= removeTextFieldProxy->mTextureCoordinatesChunkSize;
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

    size_t FontRenderData::GetVerticesCount() const
    {
        return mVerticesCount;
    }

    const std::vector<std::shared_ptr<TextFieldProxy>> &FontRenderData::GetTexFieldProxies() const
    {
        return mTextFields;
    }

    const std::shared_ptr<TextFieldProxy> &FontRenderData::GetTextFieldById(const int32_t textFieldId) const
    {
        const auto it = std::find_if(
            mTextFields.begin(), mTextFields.end(), [=](const auto &textFieldSp)
            { return textFieldSp->mTextFieldId == textFieldId; });
        assert(it != mTextFields.end());
        return *it;
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
        assert(positionVBO && textureCoordinatesVBO);

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

    void FontHandler::UnregisterText(const std::string &fontName, const int32_t textFieldProxyId)
    {
        assert(mFontRenderDataMap.count(fontName));
        mFontRenderDataMap.at(fontName)->UnregisterText(textFieldProxyId);
    }

    void FontHandler::TextPositionChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec2 &position)
    {
        assert(mFontRenderDataMap.count(fontName));
        mFontRenderDataMap.at(fontName)->TextPositionChanged(textFieldProxyId, position);
    }

    void FontHandler::TextVisibilityChanged(const std::string &fontName, const int32_t textFieldProxyId, const bool bIsVisible)
    {
        assert(mFontRenderDataMap.count(fontName));
        mFontRenderDataMap.at(fontName)->TextVisibilityChanged(textFieldProxyId, bIsVisible);
    }

    void FontHandler::TextColorChanged(const std::string &fontName, const int32_t textFieldProxyId, const glm::vec3 &color)
    {
        assert(mFontRenderDataMap.count(fontName));
        mFontRenderDataMap.at(fontName)->TextColorChanged(textFieldProxyId, color);
    }

    void FontHandler::TextChanged(const std::string &fontName, const int32_t textFieldProxyId, const std::string &text)
    {
        assert(mFontRenderDataMap.count(fontName));
        mFontRenderDataMap.at(fontName)->TextChanged(textFieldProxyId, text);
    }

    float FontHandler::GetTextWidth(const std::string &fontName, const int32_t textFieldProxyId) const
    {
        assert(mFontRenderDataMap.count(fontName));
        return mFontRenderDataMap.at(fontName)->GetTextFieldById(textFieldProxyId)->mCreatedMeshTextWidth;
    }

    float FontHandler::GetTextHeight(const std::string &fontName, const int32_t textFieldProxyId) const
    {
        assert(mFontRenderDataMap.count(fontName));
        return mFontRenderDataMap.at(fontName)->GetTextFieldById(textFieldProxyId)->mCreatedMeshTextHeight;
    }

    bool FontHandler::IsTextSubscribedOnSizeChangeUpdate(const std::string& fontName, const int32_t textFieldProxyId) const
    {
        assert(mFontRenderDataMap.count(fontName));
        return mFontRenderDataMap.at(fontName)->GetTextFieldById(textFieldProxyId)->mIsSubscribedOnTextScreenSpaceSizeUpdate;
    }

    glm::vec2 FontHandler::GetTextScreenSpaceSize(const std::string &fontName, const int32_t textFieldProxyId) const
    {
        assert(mFontRenderDataMap.count(fontName));
        const auto &textFiledSp = mFontRenderDataMap.at(fontName)->GetTextFieldById(textFieldProxyId);
        return glm::vec2(textFiledSp->mCreatedMeshTextWidth, textFiledSp->mCreatedMeshTextHeight);
    }
}