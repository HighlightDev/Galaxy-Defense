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

FreeTypeFontBatcher::FreeTypeFontBatcher(
    const std::shared_ptr<FreeTypeFontAtlas>& fontAtlas, const FreeTypeFontParams& fontParams)
    : mPositionChunkData()
    , mTextureCoordinatesChunkData()
    , mVerticesCount(0)
    , mTextFontAtlas(fontAtlas)
    , mTextFields()
    , mFontParams(fontParams)
{
}

void FreeTypeFontBatcher::RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto& textFieldSp) {
        return textFieldSp->GetTextFieldId() == textFieldProxy->GetTextFieldId();
    });
    ext_assert(
        it == mTextFields.end(),
        "Text field with ID " + std::to_string(textFieldProxy->GetTextFieldId()) + " is already registered");

    mTextFields.emplace_back(textFieldProxy);
    if (textFieldProxy->GetText() != "") // if text is empty - skip allocation
    {
        AllocateTextSpace(textFieldProxy);
    }
}

void FreeTypeFontBatcher::UnregisterText(const int32_t textFieldId)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldId);
    ext_assert(
        freeTypeTextProxy != nullptr,
        "FreeTypeFontBatcher::UnregisterText: text field proxy is null for ID " + std::to_string(textFieldId));
    mTextFields.erase(std::remove_if(
        mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldId == mProxy->GetTextFieldId(); }));

    FreeAllocatedTextSpace(freeTypeTextProxy);
}

void FreeTypeFontBatcher::TextPositionChanged(const int32_t textFieldId, const glm::vec2& position)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldId);
    ext_assert(
        freeTypeTextProxy != nullptr,
        "FreeTypeFontBatcher::TextPositionChanged: text field proxy is null for ID " + std::to_string(textFieldId));
    freeTypeTextProxy->SetPosition(position);
}

void FreeTypeFontBatcher::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    ext_assert(
        freeTypeTextProxy != nullptr,
        "FreeTypeFontBatcher::TextColorChanged: text field proxy is null for ID " + std::to_string(textFieldProxyId));
    freeTypeTextProxy->SetColor(color);
}

void FreeTypeFontBatcher::TextChanged(const int32_t textFieldProxyId, const std::string& text)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    ext_assert(
        freeTypeTextProxy != nullptr,
        "FreeTypeFontBatcher::TextChanged: text field proxy is null for ID " + std::to_string(textFieldProxyId));
    freeTypeTextProxy->SetText(text);

    ReallocateTextSpace();
}

void FreeTypeFontBatcher::TextVisibilityChanged(const int32_t textFieldProxyId, const bool isVisible)
{
    const auto freeTypeTextProxy = GetFreeTypeTextFieldById(textFieldProxyId);
    ext_assert(
        freeTypeTextProxy != nullptr,
        "FreeTypeFontBatcher::TextVisibilityChanged: text field proxy is null for ID " + std::to_string(textFieldProxyId));
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
    ext_assert(
        textFieldProxy->GetFontSize() == mTextFontAtlas->GetFontSize(),
        "Font size mismatch! Expected: " + std::to_string(mTextFontAtlas->GetFontSize())
            + ", got: " + std::to_string(textFieldProxy->GetFontSize()));

    const auto& [vertexPositions, textCoordinates] = textMeshCreator.CreateTextMesh(textFieldProxy, mTextFontAtlas);
    ext_assert(vertexPositions.size() == textCoordinates.size(), "Vertex positions and texture coordinates size mismatch");
    // positions
    const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
    textFieldProxy->SetVertexStart(positionOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize()));
    const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetElementByteSize() * positionVBO->GetVectorSize();
    ext_assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize, "Position buffer overflow");
    positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
    mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
    textFieldProxy->SetPositionChunkOffset(positionOffset);
    textFieldProxy->SetPositionChunkSize(positionSizeUpdate);
    textFieldProxy->SetVerticesCount(vertexPositions.size());

    // texture coordinates
    const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
    const size_t texCoordinatesSizeUpdate
        = textCoordinates.size() * textureCoordinatesVBO->GetElementByteSize() * textureCoordinatesVBO->GetVectorSize();
    ext_assert(
        texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize,
        "Texture coordinates buffer overflow");
    textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
    mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
    textFieldProxy->SetTextureCoordinatesChunkOffset(texCoordinatesOffset);
    textFieldProxy->SetTextureCoordinatesChunkSize(texCoordinatesSizeUpdate);
    const glm::ivec2 textWidthHeightScreenSpace = textMeshCreator.CalcTextScreenSpaceSize(textFieldProxy, mTextFontAtlas);
    textFieldProxy->SetCreatedMeshTextWidthHeightScreenSpace(textWidthHeightScreenSpace);
    textFieldProxy->SetCreatedMeshTextWidthHeightNormalized(glm::vec2(
        static_cast<float>(textWidthHeightScreenSpace.x) / static_cast<float>(displayDeviceProvider->GetWindowWidth()),
        static_cast<float>(textWidthHeightScreenSpace.y) / static_cast<float>(displayDeviceProvider->GetWindowHeight())));
}

void FreeTypeFontBatcher::AllocateTextSpace(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    if (!textFieldProxy->GetText().empty()) {
        auto* const positionVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
        auto* const textureCoordinatesVBO = mTextFontAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
        ext_assert(positionVBO && textureCoordinatesVBO, "Failed to get VBOs for vertex positions or texture coordinates");

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
    ext_assert(positionVBO && textureCoordinatesVBO, "Failed to get VBOs for vertex positions or texture coordinates");

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
    ext_assert(positionVBO && textureCoordinatesVBO, "Failed to get VBOs for vertex positions or texture coordinates");

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

const FreeTypeFontParams& FreeTypeFontBatcher::GetFontParams() const
{
    return mFontParams;
}

FreeTypeFontHandler::FreeTypeFontHandler()
    : mFontBatcherMap()
{
}

void FreeTypeFontHandler::RegisterFont(const FreeTypeFontParams& fontParams) const
{
    ext_assert(mFontBatcherMap.count(fontParams) == 0, "Font already registered: " + fontParams.FontName);
    const auto& fontTextureAtlas = FreeTypeFontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
    ext_assert(fontTextureAtlas, "Failed to get or allocate FreeTypeFontAtlas for font: " + fontParams.FontName);
    mFontBatcherMap.emplace(fontParams, std::make_shared<FreeTypeFontBatcher>(fontTextureAtlas, fontParams));

    const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;
    constexpr size_t verticesPerCharacter = 6;
    const auto& fontBatcher = mFontBatcherMap.at(fontParams);

    auto* const positionVBO = fontTextureAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
    auto* const textureCoordinatesVBO = fontTextureAtlas->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
    ext_assert(positionVBO && textureCoordinatesVBO, "Failed to get VBOs for vertex positions or texture coordinates");

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
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::UnregisterText: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    batcherSp->UnregisterText(textFieldProxyId);
    if (batcherSp->GetFreeTypeTextFieldProxies().empty()) {
        FreeTypeFontMeshPool::GetInstance()->TryToFreeMemory(batcherSp->GetFontParams());
        mFontBatcherMap.erase(batcherSp->GetFontParams());
    }
}

void FreeTypeFontHandler::TextPositionChanged(const int32_t textFieldProxyId, const glm::vec2& position)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::TextPositionChanged: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    batcherSp->TextPositionChanged(textFieldProxyId, position);
}

void FreeTypeFontHandler::TextVisibilityChanged(const int32_t textFieldProxyId, const bool bIsVisible)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::TextVisibilityChanged: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    batcherSp->TextVisibilityChanged(textFieldProxyId, bIsVisible);
}

void FreeTypeFontHandler::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::TextColorChanged: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    batcherSp->TextColorChanged(textFieldProxyId, color);
}

void FreeTypeFontHandler::TextChanged(const int32_t textFieldProxyId, const std::string& text)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::TextChanged: Font batcher not found for text field proxy ID " + std::to_string(textFieldProxyId));
    batcherSp->TextChanged(textFieldProxyId, text);
}

void FreeTypeFontHandler::FontSizeChanged(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxy->GetTextFieldId());
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::FontSizeChanged: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxy->GetTextFieldId()));
    UnregisterText(textFieldProxy->GetTextFieldId());
    RegisterText(textFieldProxy);
}

float FreeTypeFontHandler::GetTextNormalizedWidth(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::GetTextNormalizedWidth: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    return batcherSp->GetFreeTypeTextFieldById(textFieldProxyId)->GetCreatedMeshTextWidthHeightNormalized().x;
}

float FreeTypeFontHandler::GetTextNormalizedHeight(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::GetTextNormalizedHeight: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    return batcherSp->GetFreeTypeTextFieldById(textFieldProxyId)->GetCreatedMeshTextWidthHeightNormalized().y;
}

glm::vec2 FreeTypeFontHandler::GetTextSizeNormalized(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::GetTextSizeNormalized: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    const auto textFiledSp = batcherSp->GetFreeTypeTextFieldById(textFieldProxyId);
    return glm::vec2(textFiledSp->GetCreatedMeshTextWidthHeightNormalized());
}

glm::ivec2 FreeTypeFontHandler::GetTextScreenSpaceSize(const int32_t textFieldProxyId) const
{
    const auto batcherSp = FindFontBatcherByTextFieldProxyId(textFieldProxyId);
    ext_assert(
        batcherSp != nullptr,
        "FreeTypeFontHandler::GetTextScreenSpaceSize: Font batcher not found for text field proxy ID "
            + std::to_string(textFieldProxyId));
    const auto textFiledSp = batcherSp->GetFreeTypeTextFieldById(textFieldProxyId);
    return glm::vec2(textFiledSp->GetCreatedMeshTextWidthHeightScreenSpace());
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