// #include "FreeTypeFontHandler.h"

// #include  "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
// #include "Core/IoCore/FolderManager.h"
// #include "FreeTypeTextMeshCreator.h"
// #include "Core/ResourceManagerCore/Pool/FontMeshPool.h"
// #include "Core/ResourceManagerCore/Pool/TexturePool.h"
// #include "Core/UtilityCore/EngineConfigHolder.h"
// #include "TextMeshCreator.h"

// #include <algorithm>

// using namespace IO;
// using namespace Resources;
// using namespace EngineCore::DataProviders;
// using namespace EngineUtility;

// namespace EngineCore {
// TextVertexChunkData::TextVertexChunkData()
//     : mCurrentChunkOffset(0)
//     , mTotalChunkSize(0)
// {
// }

// FreeTypeFontBatcher::FreeTypeFontBatcher(
//     const std::shared_ptr<FreeTypeFontAtlas>& fontAtlas)
//     : mPositionChunkData()
//     , mTextureCoordinatesChunkData()
//     , mVerticesCount(0)
//     , mFontAtlas(fontAtlas)
// {
// }

// void FreeTypeFontBatcher::RegisterText(const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy)
// {
//     const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [&](const auto& mProxy) {
//         return textFieldProxy->GetTextFieldId() == mProxy->GetTextFieldId();
//     });
//     assert(it == mTextFields.end());
//     mTextFields.emplace_back(textFieldProxy);

//     if (textFieldProxy->GetText() != "") // if text is empty - skip allocation
//     {
//         AllocateTextSpace(textFieldProxy);
//     }
// }

// void FreeTypeFontBatcher::UnregisterText(const int32_t textFieldId)
// {
//     const auto foundIt = std::find_if(
//         mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldId == mProxy->GetTextFieldId(); });
//     assert(foundIt != mTextFields.end());

//     auto deleteTextProxy = *foundIt;
//     const auto removeIt = std::remove_if(
//         mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldId == mProxy->GetTextFieldId(); });

//     mTextFields.erase(removeIt);

//     FreeAllocatedTextSpace(deleteTextProxy);
// }

// void FreeTypeFontBatcher::TextPositionChanged(const int32_t textFieldId, const glm::vec2& position)
// {
//     const auto foundIt = std::find_if(
//         mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldId == mProxy->GetTextFieldId(); });
//     if (foundIt != mTextFields.end()) {
//         (*foundIt)->SetPosition(position);
//     }
// }

// void FreeTypeFontBatcher::TextColorChanged(const int32_t textFieldProxyId, const glm::vec3& color)
// {
//     const auto foundIt = std::find_if(
//         mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldProxyId == mProxy->GetTextFieldId(); });
//     if (foundIt != mTextFields.end()) {
//         (*foundIt)->SetColor(color);
//     }
// }

// void FreeTypeFontBatcher::TextChanged(const int32_t textFieldProxyId, const std::string& text)
// {
//     const auto foundIt = std::find_if(
//         mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldProxyId == mProxy->GetTextFieldId(); });
//     if (foundIt != mTextFields.end()) {
//         (*foundIt)->SetText(text);
//     }

//     ReallocateTextSpace();
// }

// void FreeTypeFontBatcher::TextVisibilityChanged(const int32_t textFieldProxyId, const bool isVisible)
// {
//     const auto foundIt = std::find_if(
//         mTextFields.begin(), mTextFields.end(), [=](const auto& mProxy) { return textFieldProxyId == mProxy->GetTextFieldId(); });
//     if (foundIt != mTextFields.end() && (*foundIt)->GetText() != "") {
//         (*foundIt)->SetIsVisible(isVisible);
//     }
// }

// void FreeTypeFontBatcher::FontBufferSubData(
//     const std::shared_ptr<FreeTypeTextFieldProxy>& textFieldProxy,
//     VertexBufferObjectBase* const positionVBO,
//     VertexBufferObjectBase* const textureCoordinatesVBO)
// {
//     FreeTypeTextMeshCreator textMeshCreator;
//     const auto& points = textMeshCreator.CreateTextMesh(textFieldProxy);
//     auto textMesh = textMeshCreator.CreateTextMesh(textFieldProxy);
//     const auto& vertexPositions = textMesh.mVertexPositions;
//     const auto& textCoordinates = textMesh.mTextureCoords;
// ////////////////////
//     // positions
//     const size_t positionOffset = mPositionChunkData.mCurrentChunkOffset;
//     textFieldProxy->SetVertexStart(positionOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize()));
//     const size_t positionSizeUpdate = vertexPositions.size() * positionVBO->GetElementByteSize();
//     assert(positionOffset + positionSizeUpdate <= mPositionChunkData.mTotalChunkSize);
//     positionVBO->BindVBO();
//     positionVBO->BufferSubData(positionOffset, positionSizeUpdate, vertexPositions.data());
//     mPositionChunkData.mCurrentChunkOffset = positionOffset + positionSizeUpdate;
//     textFieldProxy->SetPositionChunkOffset(positionOffset);
//     textFieldProxy->SetPositionChunkSize(positionSizeUpdate);
//     textFieldProxy->SetVerticesCount(
//         mPositionChunkData.mCurrentChunkOffset / (positionVBO->GetElementByteSize() * positionVBO->GetVectorSize())
//         - textFieldProxy->GetVertexStart());

//     // texture coordinates
//     const size_t texCoordinatesOffset = mTextureCoordinatesChunkData.mCurrentChunkOffset;
//     const size_t texCoordinatesSizeUpdate = textCoordinates.size() * textureCoordinatesVBO->GetElementByteSize();
//     assert(texCoordinatesOffset + texCoordinatesSizeUpdate <= mTextureCoordinatesChunkData.mTotalChunkSize);
//     textureCoordinatesVBO->BindVBO();
//     textureCoordinatesVBO->BufferSubData(texCoordinatesOffset, texCoordinatesSizeUpdate, textCoordinates.data());
//     mTextureCoordinatesChunkData.mCurrentChunkOffset = texCoordinatesOffset + texCoordinatesSizeUpdate;
//     textFieldProxy->SetTextureCoordinatesChunkOffset(texCoordinatesOffset);
//     textFieldProxy->SetTextureCoordinatesChunkSize(texCoordinatesSizeUpdate);
//     textFieldProxy->SetCreatedMeshTextWidth(textMesh.mTextWidth);
//     textFieldProxy->SetCreatedMeshTextHeight(textMesh.mTextHeight);
// }

// void FreeTypeFontBatcher::AllocateTextSpace(const std::shared_ptr<TextFieldProxy>& textFieldProxy)
// {
//     auto* const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
//     auto* const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
//     assert(positionVBO && textureCoordinatesVBO);

//     FontBufferSubData(textFieldProxy, positionVBO, textureCoordinatesVBO);
//     textureCoordinatesVBO->UnbindVBO();

//     mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
// }

// void FreeTypeFontBatcher::ReallocateTextSpace()
// {
//     auto* const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
//     auto* const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
//     assert(positionVBO && textureCoordinatesVBO);

//     mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
//     mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

//     for (auto& textProxy : mTextFields) {
//         if (textProxy->GetText() != "") // if text is empty - skip allocation
//         {
//             FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
//         }
//     }
// }

// void FreeTypeFontBatcher::FreeAllocatedTextSpace(const std::shared_ptr<TextFieldProxy>& removeTextFieldProxy)
// {
//     auto* const positionVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
//     auto* const textureCoordinatesVBO = mTextMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
//     assert(positionVBO && textureCoordinatesVBO);

//     if (0 == removeTextFieldProxy->GetPositionChunkOffset()) // text that should be removed is at the beginning
//     {
//         mPositionChunkData.mCurrentChunkOffset = 0; // start filling buffer from the beginning
//         mTextureCoordinatesChunkData.mCurrentChunkOffset = 0;

//         for (auto& textProxy : mTextFields) {
//             FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
//         }
//     } else {
//         std::vector<std::shared_ptr<TextFieldProxy>> textFieldProxiesToReallocate;
//         std::copy_if(
//             mTextFields.begin(), mTextFields.end(), std::back_inserter(textFieldProxiesToReallocate), [&](const auto& textField) {
//                 return textField->GetPositionChunkOffset() > removeTextFieldProxy->GetPositionChunkOffset();
//             });

//         if (textFieldProxiesToReallocate.size()) // if need to reallocate for existing text
//         {
//             mPositionChunkData.mCurrentChunkOffset = removeTextFieldProxy->GetPositionChunkOffset();
//             mTextureCoordinatesChunkData.mCurrentChunkOffset = removeTextFieldProxy->GetTextureCoordinatesChunkOffset();
//             for (auto& textProxy : textFieldProxiesToReallocate) {
//                 FontBufferSubData(textProxy, positionVBO, textureCoordinatesVBO);
//             }
//         } else {
//             mPositionChunkData.mCurrentChunkOffset -= removeTextFieldProxy->GetPositionChunkSize();
//             mTextureCoordinatesChunkData.mCurrentChunkOffset -= removeTextFieldProxy->GetTextureCoordinatesChunkSize();
//         }
//     }

//     textureCoordinatesVBO->UnbindVBO();
//     mVerticesCount = (mPositionChunkData.mCurrentChunkOffset / positionVBO->GetElementByteSize()) / positionVBO->GetVectorSize();
// }

// const std::shared_ptr<TextMesh>& FreeTypeFontBatcher::GetTextMesh() const
// {
//     return mTextMesh;
// }

// const std::shared_ptr<ITexture>& FreeTypeFontBatcher::GetFontTextureAtlas() const
// {
//     return mFontTextureAtlas;
// }

// const std::shared_ptr<FontMetaFile>& FreeTypeFontBatcher::GetFontMetaFile() const
// {
//     return mFontMetaFile;
// }

// TextVertexChunkData& FreeTypeFontBatcher::GetPositionChunkDataRef()
// {
//     return mPositionChunkData;
// }

// TextVertexChunkData& FreeTypeFontBatcher::GetTextureCoordinatesChunkDataRef()
// {
//     return mTextureCoordinatesChunkData;
// }

// size_t FreeTypeFontBatcher::GetVerticesCount() const
// {
//     return mVerticesCount;
// }

// const std::vector<std::shared_ptr<TextFieldProxy>>& FreeTypeFontBatcher::GetTexFieldProxies() const
// {
//     return mTextFields;
// }

// const std::shared_ptr<TextFieldProxy>& FreeTypeFontBatcher::GetTextFieldById(const int32_t textFieldId) const
// {
//     const auto it = std::find_if(mTextFields.begin(), mTextFields.end(), [=](const auto& textFieldSp) {
//         return textFieldSp->GetTextFieldId() == textFieldId;
//     });
//     assert(it != mTextFields.end());
//     return *it;
// }

// FreeTypeFontHandler::FreeTypeFontHandler()
//     : mFontBatcherMap()
// {
// }

// void FreeTypeFontHandler::RegisterFont(const FontParams& fontParams)
// {
//     assert(mFontBatcherMap.count(fontParams.FontName) == 0);

//     const auto& fontMesh = FontMeshPool::GetInstance()->GetOrAllocateResource(fontParams);
//     const auto& fontTextureAtlas = TexturePool::GetInstance()->GetOrAllocateResource(fontParams.FontTextureAtlas);
//     const auto& fontDescriptorFile = std::make_shared<FontMetaFile>(
//         FolderManager::GetInstance()->GetFontsPath() + fontParams.FontDescriptorFile,
//         GeneralSystemSettingsDataProvider::GetInstance()->GetWidthToHeightRatio());
//     mFontBatcherMap.emplace(fontParams.FontName, std::make_shared<FreeTypeFontBatcher>(fontMesh, fontTextureAtlas, fontDescriptorFile));

//     const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;
//     static constexpr size_t verticesPerCharacter = 6;
//     const auto& fontBatcher = mFontBatcherMap.at(fontParams.FontName);

//     auto* const positionVBO = fontMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexPosition");
//     auto* const textureCoordinatesVBO = fontMesh->GetBuffer()->GetVboByAttribArrayIndexName("VertexTexCoords");
//     assert(positionVBO && textureCoordinatesVBO);

//     fontBatcher->GetPositionChunkDataRef().mTotalChunkSize
//         = maxFontCharactersCount * verticesPerCharacter * positionVBO->GetVectorSize() * positionVBO->GetElementByteSize();

//     fontBatcher->GetTextureCoordinatesChunkDataRef().mTotalChunkSize = maxFontCharactersCount * verticesPerCharacter
//         * textureCoordinatesVBO->GetVectorSize() * textureCoordinatesVBO->GetElementByteSize();
// }

// std::shared_ptr<FreeTypeFontBatcher> FreeTypeFontHandler::GetFontBatcher(const std::string& fontName) const
// {
//     return mFontBatcherMap.count(fontName) ? mFontBatcherMap.at(fontName) : nullptr;
// }

// const std::unordered_map<std::string, std::shared_ptr<FreeTypeFontBatcher>>& FreeTypeFontHandler::GetFontBatcher() const
// {
//     return mFontBatcherMap;
// }

// void FreeTypeFontHandler::RegisterText(const std::shared_ptr<TextFieldProxy>& textFieldProxy)
// {
//     assert(mFontBatcherMap.count(textFieldProxy->GetFontName()));
//     mFontBatcherMap.at(textFieldProxy->GetFontName())->RegisterText(textFieldProxy);
// }

// void FreeTypeFontHandler::UnregisterText(const std::string& fontName, const int32_t textFieldProxyId)
// {
//     assert(mFontBatcherMap.count(fontName));
//     mFontBatcherMap.at(fontName)->UnregisterText(textFieldProxyId);
// }

// void FreeTypeFontHandler::TextPositionChanged(const std::string& fontName, const int32_t textFieldProxyId, const glm::vec2& position)
// {
//     assert(mFontBatcherMap.count(fontName));
//     mFontBatcherMap.at(fontName)->TextPositionChanged(textFieldProxyId, position);
// }

// void FreeTypeFontHandler::TextVisibilityChanged(const std::string& fontName, const int32_t textFieldProxyId, const bool bIsVisible)
// {
//     assert(mFontBatcherMap.count(fontName));
//     mFontBatcherMap.at(fontName)->TextVisibilityChanged(textFieldProxyId, bIsVisible);
// }

// void FreeTypeFontHandler::TextColorChanged(const std::string& fontName, const int32_t textFieldProxyId, const glm::vec3& color)
// {
//     assert(mFontBatcherMap.count(fontName));
//     mFontBatcherMap.at(fontName)->TextColorChanged(textFieldProxyId, color);
// }

// void FreeTypeFontHandler::TextChanged(const std::string& fontName, const int32_t textFieldProxyId, const std::string& text)
// {
//     assert(mFontBatcherMap.count(fontName));
//     mFontBatcherMap.at(fontName)->TextChanged(textFieldProxyId, text);
// }

// float FreeTypeFontHandler::GetTextWidth(const std::string& fontName, const int32_t textFieldProxyId) const
// {
//     assert(mFontBatcherMap.count(fontName));
//     return mFontBatcherMap.at(fontName)->GetTextFieldById(textFieldProxyId)->GetCreatedMeshTextWidth();
// }

// float FreeTypeFontHandler::GetTextHeight(const std::string& fontName, const int32_t textFieldProxyId) const
// {
//     assert(mFontBatcherMap.count(fontName));
//     return mFontBatcherMap.at(fontName)->GetTextFieldById(textFieldProxyId)->GetCreatedMeshTextHeight();
// }

// bool FreeTypeFontHandler::IsTextSubscribedOnSizeChangeUpdate(const std::string& fontName, const int32_t textFieldProxyId) const
// {
//     assert(mFontBatcherMap.count(fontName));
//     return mFontBatcherMap.at(fontName)->GetTextFieldById(textFieldProxyId)->GetIsSubscribedOnTextScreenSpaceSizeUpdate();
// }

// glm::vec2 FreeTypeFontHandler::GetTextScreenSpaceSize(const std::string& fontFileName, const int32_t textFieldProxyId) const
// {
//     assert(mFontBatcherMap.count(fontFileName));
//     const auto& textFiledSp = mFontBatcherMap.at(fontFileName)->GetTextFieldById(textFieldProxyId);
//     return glm::vec2(textFiledSp->GetCreatedMeshTextWidth(), textFiledSp->GetCreatedMeshTextHeight());
// }
// } // namespace EngineCore