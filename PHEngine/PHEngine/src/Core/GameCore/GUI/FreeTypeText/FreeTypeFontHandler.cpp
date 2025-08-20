#include "FreeTypeFontHandler.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObjectBase.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/FreeTypeFontMeshPool.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "FreeTypeFontAtlas.h"
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
    const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto& textFieldSp) {
        return textFieldSp->GetTextFieldId() == textFieldProxy->GetTextFieldId();
    });
    assert(it == mTextFields.end());

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
    mTextFields.erase(std::remove_if(
        mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldId == mProxy->GetTextFieldId(); }));

    FreeAllocatedTextSpace(freeTypeTextProxy);
}

void FreeTypeFontBatcher::TextPositionChanged(const int32_t textFieldId, const glm::vec2& position)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldId);
    assert(freeTypeTextProxy != nullptr);
    freeTypeTextProxy->SetPosition(position);
}

void FreeTypeFontBatcher::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    assert(freeTypeTextProxy != nullptr);
    freeTypeTextProxy->SetColor(color);
}

void FreeTypeFontBatcher::TextChanged(const int32_t textFieldProxyId, const std::string& text)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    assert(freeTypeTextProxy != nullptr);
    freeTypeTextProxy->SetText(text);

    ReallocateTextSpace();
}

void FreeTypeFontBatcher::TextVisibilityChanged(const int32_t textFieldProxyId, const bool isVisible)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    assert(freeTypeTextProxy != nullptr);
    if (freeTypeTextProxy && freeTypeTextProxy->GetText() != "") {
        freeTypeTextProxy->SetIsVisible(isVisible);
    }
}

void FreeTypeFontBatcher::FontBufferSubData(
    const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy,
    VertexBufferObjectBase* const positionVBO,
    VertexBufferObjectBase* const textureCoordinatesVBO)
{
    const auto displayDeviceProvider = EngineCore::DataProviders::GeneralSystemSettingsDataProvider::GetInstance();

    FreeTypeTextMeshCreator textMeshCreator;
    const auto& [vertexPositions, textCoordinates] = textMeshCreator.CreateTextMesh(textFieldProxy, mTextFontAtlas);
    assert(vertexPositions.size() == textCoordinates.size());
    // positions
    const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
    textFieldProxy->SetVertexStart(positionOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize()));
    const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetElementByteSize() * positionVBO->GetVectorSize();
    assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
    positionVBO->BindVBO();
    positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
    mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
    textFieldProxy->SetPositionChunkOffset(positionOffset);
    textFieldProxy->SetPositionChunkSize(positionSizeUpdate);
    textFieldProxy->SetVerticesCount(vertexPositions.size());

    // texture coordinates
    const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
    const size_t texCoordinatesSizeUpdate
        = textCoordinates.size() * textureCoordinatesVBO->GetElementByteSize() * textureCoordinatesVBO->GetVectorSize();
    assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
    textureCoordinatesVBO->BindVBO();
    textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
    mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
    textFieldProxy->SetTextureCoordinatesChunkOffset(texCoordinatesOffset);
    textFieldProxy->SetTextureCoordinatesChunkSize(texCoordinatesSizeUpdate);
    const glm::vec2 textWidthHeightScreenSpace = textMeshCreator.CalcTextScreenSpaceSize(textFieldProxy, mTextFontAtlas);
    textFieldProxy->SetCreatedMeshTextWidthTextureSpace(
        static_cast<float>(textWidthHeightScreenSpace.x) / static_cast<float>(displayDeviceProvider->GetWindowWidth()));
    textFieldProxy->SetCreatedMeshTextHeightTextureSpace(
        static_cast<float>(textWidthHeightScreenSpace.y) / static_cast<float>(displayDeviceProvider->GetWindowHeight()));
}

void FreeTypeFontBatcher::AllocateTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    if (!textFieldProxy->GetText().empty()) {
        auto* const positionVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
        auto* const textureCoordinatesVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
        assert(positionVBO && textureCoordinatesVBO);

        FontBufferSubData(textFieldProxy, positionVBO, textureCoordinatesVBO);
        textureCoordinatesVBO->UnbindVBO();

        mVerticesCount
            = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
    }
}

void FreeTypeFontBatcher::ReallocateTextSpace()
{
    auto* const positionVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    assert(positionVBO && textureCoordinatesVBO);

    mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
    mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

    for (auto& textProxy : mTextFields) {
        if (!textProxy->GetText().empty()) { // if text is empty - skip allocation
            FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
        }
    }

    mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
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
            if (!textProxy->GetText().empty()) {
                FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
            }
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
                if (!textProxy->GetText().empty()) {
                    FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
                }
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
    return it != mTextFields.end() ? *it : nullptr;
}

FreeTypeFontHandler::FreeTypeFontHandler()
    : mFontBatcherMap()
{
}

void FreeTypeFontHandler::RegisterFont(const FreeTypeFontParams& fontParams) const
{
    assert(mFontBatcherMap.count(fontParams) == 0);
    const auto& fontTextureAtlas = FreeTypeFontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
    assert(fontTextureAtlas);
    mFontBatcherMap.emplace(fontParams, std::make_shared<FreeTypeFontBatcher>(fontTextureAtlas));

    const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;
    constexpr size_t verticesPerCharacter = 6;
    const auto& fontBatcher = mFontBatcherMap.at(fontParams);

    auto* const positionVBO = fontTextureAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = fontTextureAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    assert(positionVBO && textureCoordinatesVBO);

    fontBatcher->GetPositionChunkDataRef().mTotalChunkSize
        = maxFontCharactersCount * verticesPerCharacter * positionVBO->GetVectorSize() * positionVBO->GetElementByteSize();

    fontBatcher->GetTextureCoordinatesChunkDataRef().mTotalChunkSize = maxFontCharactersCount * verticesPerCharacter
        * textureCoordinatesVBO->GetVectorSize() * textureCoordinatesVBO->GetElementByteSize();
}

std::shared_ptr<FreeTypeFontBatcher> FreeTypeFontHandler::GetFontBatcher(const FreeTypeFontParams& fontParams) const
{
    return mFontBatcherMap.count(fontParams) ? mFontBatcherMap.at(fontParams) : nullptr;
}

const std::unordered_map<FreeTypeFontParams, std::shared_ptr<FreeTypeFontBatcher>>& FreeTypeFontHandler::GetFontBatcherMap() const
{
    return mFontBatcherMap;
}

void FreeTypeFontHandler::RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    if (!mFontBatcherMap.count(textFieldProxy->GetFontParams())) {
        RegisterFont(textFieldProxy->GetFontParams());
    }
    mFontBatcherMap.at(textFieldProxy->GetFontParams())->RegisterText(textFieldProxy);
}

void FreeTypeFontHandler::UnregisterText(const int32_t textFieldProxyId)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    batcherSp->UnregisterText(textFieldProxyId);
}

void FreeTypeFontHandler::TextPositionChanged(const int32_t textFieldProxyId, const glm::vec2& position)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    batcherSp->TextPositionChanged(textFieldProxyId, position);
}

void FreeTypeFontHandler::TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    batcherSp->TextVisibilityChanged(textFieldProxyId, bIsVisible);
}

void FreeTypeFontHandler::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    batcherSp->TextColorChanged(textFieldProxyId, color);
}

void FreeTypeFontHandler::TextChanged(const int32_t textFieldProxyId, const std::string& text)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    batcherSp->TextChanged(textFieldProxyId, text);
}

void FreeTypeFontHandler::FontSizeChanged(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxy->GetTextFieldId());
    assert(batcherSp != nullptr);
    UnregisterText(textFieldProxy->GetTextFieldId());
    RegisterText(textFieldProxy);
}

float FreeTypeFontHandler::GetTextWidth(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    return batcherSp->GetFreeTypeTextFieldById(textFieldProxyId)->GetCreatedMeshTextWidthTextureSpace();
}

float FreeTypeFontHandler::GetTextHeight(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    return batcherSp->GetFreeTypeTextFieldById(textFieldProxyId)->GetCreatedMeshTextHeightTextureSpace();
}

bool FreeTypeFontHandler::IsTextSubscribedOnSizeChangeUpdate(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    return batcherSp->GetFreeTypeTextFieldById(textFieldProxyId)->GetIsSubscribedOnTextScreenSpaceSizeUpdate();
}

glm::vec2 FreeTypeFontHandler::GetTextScreenSpaceSize(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    assert(batcherSp != nullptr);
    const auto textFiledSp = batcherSp->GetFreeTypeTextFieldById(textFieldProxyId);
    return glm::vec2(textFiledSp->GetCreatedMeshTextWidthTextureSpace(), textFiledSp->GetCreatedMeshTextHeightTextureSpace());
}

std::shared_ptr<FreeTypeFontBatcher> FreeTypeFontHandler::FindFontBatcherByTextFieldProxyId(const int32_t proxyId) const
{
    for (const auto& [key, batcher] : mFontBatcherMap) {
        if (const auto resultProxy = batcher->GetFreeTypeTextFieldById(proxyId)) {
            return batcher;
        }
    }
    return nullptr;
}
} // namespace EngineCore::GUI