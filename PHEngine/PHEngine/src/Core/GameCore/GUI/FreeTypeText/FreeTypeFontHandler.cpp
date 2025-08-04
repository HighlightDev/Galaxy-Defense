#include "FreeTypeFontHandler.h"

#include "Core/GraphicsCore/OpenGL/VertexBufferObjectBase.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/FreeTypeFontMeshPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "FreeTypeFontAtlas.h"
#include "FreeTypeFontParams.h"
#include "FreeTypeTextFieldProxy.h"
#include "FreeTypeTextMeshCreator.h"

#include <algorithm>

using namespace IO;
using namespace Resources;
using namespace EngineUtility;
using namespace Graphics::OpenGL;

namespace EngineCore::GUI {
FreeTypeTextVertexChunkData::FreeTypeTextVertexChunkData()
    : mCurrentChunkOffset(0)
    , mTotalChunkSize(0)
{
}

FreeTypeFontBatcher::FreeTypeFontBatcher(const std::shared_ptr<FreeTypeFontAtlas>& fontAtlas)
    : mPositionChunkData()
    , mTextureCoordinatesChunkData()
    , mVerticesCount(0)
    , mTextFontAtlas(fontAtlas)
{
}

void FreeTypeFontBatcher::RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    const auto foundTextFieldProxy = GetFreeTypeTextFieldById(textFieldProxy->GetTextFieldId());
    assert(foundTextFieldProxy == nullptr);
    mTextFields.emplace_back(textFieldProxy);

    if (textFieldProxy->GetText() != "") // if text is empty - skip allocation
    {
        AllocateTextSpace(textFieldProxy);
    }
}

void FreeTypeFontBatcher::UnregisterText(const int32_t textFieldId)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldId);
    assert(freeTypeTextProxy != nullptr);
    mTextFields.erase(std::remove_if(mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) {
        return textFieldId == mProxy->GetTextFieldId();
    }));

    FreeAllocatedTextSpace(freeTypeTextProxy);
}

void FreeTypeFontBatcher::TextPositionChanged(const int32_t textFieldId, const glm::vec2& position)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldId);
    if (freeTypeTextProxy) {
        freeTypeTextProxy->SetPosition(position);
    }
}

void FreeTypeFontBatcher::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    if (freeTypeTextProxy) {
        freeTypeTextProxy->SetColor(color);
    }
}

void FreeTypeFontBatcher::TextChanged(const int32_t textFieldProxyId, const std::string& text)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    if (freeTypeTextProxy) {
        freeTypeTextProxy->SetText(text);
    }

    ReallocateTextSpace();
}

void FreeTypeFontBatcher::TextVisibilityChanged(const int32_t textFieldProxyId, const bool isVisible)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    if (freeTypeTextProxy && freeTypeTextProxy->GetText() != "") {
        freeTypeTextProxy->SetIsVisible(isVisible);
    }
}

void FreeTypeFontBatcher::FontBufferSubData(
    const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy,
    VertexBufferObjectBase* const positionVBO,
    VertexBufferObjectBase* const textureCoordinatesVBO)
{
    FreeTypeTextMeshCreator textMeshCreator;
    const auto& [vertexPositions, textCoordinates] = textMeshCreator.CreateTextMesh(textFieldProxy, mTextFontAtlas);
    assert(vertexPositions.size() == textCoordinates.size());
    // positions
    const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
    textFieldProxy->SetVertexStart(positionOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize()));
    const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetElementByteSize();
    assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
    positionVBO->BindVBO();
    positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
    mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
    textFieldProxy->SetPositionChunkOffset(positionOffset);
    textFieldProxy->SetPositionChunkSize(positionSizeUpdate);
    textFieldProxy->SetVerticesCount(
        mPositionChunkData.mCurrentChunkOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize())
        - textFieldProxy->GetVertexStart());

    // texture coordinates
    const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
    const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetElementByteSize();
    assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
    textureCoordinatesVBO->BindVBO();
    textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
    mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
    textFieldProxy->SetTextureCoordinatesChunkOffset(texCoordinatesOffset);
    textFieldProxy->SetTextureCoordinatesChunkSize(texCoordinatesSizeUpdate);
    textFieldProxy->SetCreatedMeshTextWidth(textMeshCreator.CalcWidth(textFieldProxy->GetText(), mTextFontAtlas));
    textFieldProxy->SetCreatedMeshTextHeight(textMeshCreator.CalcHeight(mTextFontAtlas));
}

void FreeTypeFontBatcher::AllocateTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    auto* const positionVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    assert(positionVBO && textureCoordinatesVBO);

    FontBufferSubData(textFieldProxy, positionVBO, textureCoordinatesVBO);
    textureCoordinatesVBO->UnbindVBO();

    mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
}

void FreeTypeFontBatcher::ReallocateTextSpace()
{
    auto* const positionVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    assert(positionVBO && textureCoordinatesVBO);

    mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
    mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

    for (auto& textProxy : mTextFields) {
        if (textProxy->GetText() != "") // if text is empty - skip allocation
        {
            FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
        }
    }
}

void FreeTypeFontBatcher::FreeAllocatedTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& removeTextFieldProxy)
{
    auto* const positionVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    assert(positionVBO && textureCoordinatesVBO);

    if (0 == removeTextFieldProxy->GetPositionChunkOffset()) // text that should be removed is at the beginning
    {
        mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
        mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

        for (auto& textProxy : mTextFields) {
            FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
        }
    } else {
        std::vector<std::shared_ptr<FreeTypeTextFieldProxy>> textFieldProxiesToReallocate;
        std::copy_if(
            mTextFields.begin(), mTextFields.end(), std::back_inserter(textFieldProxiesToReallocate), [&](const auto& textField) {
                return textField->GetPositionChunkOffset() > removeTextFieldProxy->GetPositionChunkOffset();
            });

        if (textFieldProxiesToReallocate.size()) // if need to reallocate for existing text
        {
            mPositionChunkData.mCurrentChunkOffset = removeTextFieldProxy->GetPositionChunkOffset();
            mTextureCoordinatesChunkData.mCurrentChunkOffset = removeTextFieldProxy->GetTextureCoordinatesChunkOffset();
            for (auto& textProxy : textFieldProxiesToReallocate) {
                FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
            }
        } else {
            mPositionChunkData.mCurrentChunkOffset -= removeTextFieldProxy->GetPositionChunkSize();
            mTextureCoordinatesChunkData.mCurrentChunkOffset -= removeTextFieldProxy->GetTextureCoordinatesChunkSize();
        }
    }

    textureCoordinatesVBO->UnbindVBO();
    mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
}

const std::shared_ptr<FreeTypeFontAtlas>& FreeTypeFontBatcher::GetFreeTypeFontAtlas() const
{
    return mTextFontAtlas;
}

std::shared_ptr<ITexture> FreeTypeFontBatcher::GetFontTextureAtlas() const
{
    return mTextFontAtlas->GetFontTextureAtlas();
}

FreeTypeTextVertexChunkData& FreeTypeFontBatcher::GetPositionChunkDataRef()
{
    return mPositionChunkData;
}

FreeTypeTextVertexChunkData& FreeTypeFontBatcher::GetTextureCoordinatesChunkDataRef()
{
    return mTextureCoordinatesChunkData;
}

size_t FreeTypeFontBatcher::GetVerticesCount() const
{
    return mVerticesCount;
}

const std::vector<std::shared_ptr<FreeTypeTextFieldProxy>>& FreeTypeFontBatcher::GetFreeTypeTextFieldProxies() const
{
    return mTextFields;
}

std::shared_ptr<FreeTypeTextFieldProxy> FreeTypeFontBatcher::GetFreeTypeTextFieldById(const int32_t textFieldId) const
{
    const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto& textFieldSp) {
        return textFieldSp->GetTextFieldId() == textFieldId;
    });
    assert(it != mTextFields.end());
    return *it;
}

FreeTypeFontHandler::FreeTypeFontHandler()
    : mFontBatcherMap()
{
}

void FreeTypeFontHandler::RegisterFont(const std::string& fontFileName)
{
    assert(mFontBatcherMap.count(fontFileName) == 0);

    constexpr int32_t s_fontSize = 16; // Default font size
    FreeTypeFontParams fontParams(fontFileName, s_fontSize);

    const auto& fontTextureAtlas = FreeTypeFontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
    assert(fontTextureAtlas);
    mFontBatcherMap.emplace(fontParams.FontDescriptorFile, std::make_shared<FreeTypeFontBatcher>(fontTextureAtlas));

    const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;
    constexpr size_t verticesPerCharacter = 6;
    const auto& fontBatcher = mFontBatcherMap.at(fontParams.FontDescriptorFile);

    auto* const positionVBO = fontTextureAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = fontTextureAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    assert(positionVBO && textureCoordinatesVBO);

    fontBatcher->GetPositionChunkDataRef().mTotalChunkSize
        = maxFontCharactersCount * verticesPerCharacter * positionVBO->GetVectorSize() * positionVBO->GetElementByteSize();

    fontBatcher->GetTextureCoordinatesChunkDataRef().mTotalChunkSize = maxFontCharactersCount * verticesPerCharacter
        * textureCoordinatesVBO->GetVectorSize() * textureCoordinatesVBO->GetElementByteSize();
}

std::shared_ptr<FreeTypeFontBatcher> FreeTypeFontHandler::GetFontBatcher(const std::string& fontName) const
{
    return mFontBatcherMap.count(fontName) ? mFontBatcherMap.at(fontName) : nullptr;
}

const std::unordered_map<std::string, std::shared_ptr<FreeTypeFontBatcher>>& FreeTypeFontHandler::GetFontBatcher() const
{
    return mFontBatcherMap;
}

void FreeTypeFontHandler::RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    assert(mFontBatcherMap.count(textFieldProxy->GetFontFileName()));
    mFontBatcherMap.at(textFieldProxy->GetFontFileName())->RegisterText(textFieldProxy);
}

void FreeTypeFontHandler::UnregisterText(const std::string& fontName, const int32_t textFieldProxyId)
{
    assert(mFontBatcherMap.count(fontName));
    mFontBatcherMap.at(fontName)->UnregisterText(textFieldProxyId);
}

void FreeTypeFontHandler::TextPositionChanged(
    const std::string& fontName, const int32_t textFieldProxyId, const glm::vec2& position)
{
    assert(mFontBatcherMap.count(fontName));
    mFontBatcherMap.at(fontName)->TextPositionChanged(textFieldProxyId, position);
}

void FreeTypeFontHandler::TextVisibilityChanged(
    const std::string& fontName, const int32_t textFieldProxyId, const bool bIsVisible)
{
    assert(mFontBatcherMap.count(fontName));
    mFontBatcherMap.at(fontName)->TextVisibilityChanged(textFieldProxyId, bIsVisible);
}

void FreeTypeFontHandler::TextColorChanged(const std::string& fontName, const int32_t textFieldProxyId, const glm::vec3& color)
{
    assert(mFontBatcherMap.count(fontName));
    mFontBatcherMap.at(fontName)->TextColorChanged(textFieldProxyId, color);
}

void FreeTypeFontHandler::TextChanged(const std::string& fontName, const int32_t textFieldProxyId, const std::string& text)
{
    assert(mFontBatcherMap.count(fontName));
    mFontBatcherMap.at(fontName)->TextChanged(textFieldProxyId, text);
}

float FreeTypeFontHandler::GetTextWidth(const std::string& fontName, const int32_t textFieldProxyId) const
{
    assert(mFontBatcherMap.count(fontName));
    return mFontBatcherMap.at(fontName)->GetFreeTypeTextFieldById(textFieldProxyId)->GetCreatedMeshTextWidth();
}

float FreeTypeFontHandler::GetTextHeight(const std::string& fontName, const int32_t textFieldProxyId) const
{
    assert(mFontBatcherMap.count(fontName));
    return mFontBatcherMap.at(fontName)->GetFreeTypeTextFieldById(textFieldProxyId)->GetCreatedMeshTextHeight();
}

bool FreeTypeFontHandler::IsTextSubscribedOnSizeChangeUpdate(const std::string& fontName, const int32_t textFieldProxyId) const
{
    assert(mFontBatcherMap.count(fontName));
    return mFontBatcherMap.at(fontName)->GetFreeTypeTextFieldById(textFieldProxyId)->GetIsSubscribedOnTextScreenSpaceSizeUpdate();
}

glm::ivec2 FreeTypeFontHandler::GetTextScreenSpaceSize(const std::string& fontFileName, const int32_t textFieldProxyId) const
{
    assert(mFontBatcherMap.count(fontFileName));
    const auto& textFiledSp = mFontBatcherMap.at(fontFileName)->GetFreeTypeTextFieldById(textFieldProxyId);
    return glm::ivec2(textFiledSp->GetCreatedMeshTextWidth(), textFiledSp->GetCreatedMeshTextHeight());
}
} // namespace EngineCore::GUI