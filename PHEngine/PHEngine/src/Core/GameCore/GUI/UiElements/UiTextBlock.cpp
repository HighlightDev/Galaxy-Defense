#include "UiTextBlock.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/UiTextBlockLuaProxy.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"
#include "Core/GraphicsCore/Renderer/SceneRenderer.h"
#include "Core/GraphicsCore/UiSceneProxy/UiTextBlockSceneProxy.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::DataProviders;
using namespace EngineCore::Scripts;
using namespace Graphics::Proxy;
using namespace Graphics::Renderer;
using namespace Resources;

namespace EngineCore {
namespace GUI {
UiTextBlock::UiTextBlock(const std::string& fontName, const std::string& name)
    : UiItemBase(name)
    , mFontName(fontName)
    , mTextLineWidthHeight(0)
{
    ext_assert(!mFontName.empty(), "Font name is empty.");
}

UiTextBlock::~UiTextBlock()
{
    LogInfo("UiTextBlock::dctor");
}

void UiTextBlock::OnRegistered()
{
    if (const auto& sceneSp = GetScene().lock()) {
        if (const auto& sceneRendererSp = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {
            if (const auto& parentCanvasSp = mParentCanvas.lock()) {
                const auto thisSceneProxy = CreateUiSceneProxy();
                sceneRendererSp->RegisterUiSceneProxy_OnRenderThread(
                    std::static_pointer_cast<UiTextBlock>(shared_from_this()), thisSceneProxy, parentCanvasSp->GetUId());
            }
        }
    }
}

void UiTextBlock::OnUnregistered()
{
}

void UiTextBlock::OnPropertiesShouldBeUpdatedOnRenderThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnRenderThread();
    mTextLineWidthHeight = GetBoundingArea().GetHalfExtent() * 2;
    SyncDataOnRenderThread();
}

void UiTextBlock::OnPropertiesShouldBeUpdatedOnLuaThread()
{
    UiItemBase::OnPropertiesShouldBeUpdatedOnLuaThread();
    SyncDataOnLuaThread();
}

std::string UiTextBlock::GetUiTypeString() const
{
    return "UiTextBlock";
}

void UiTextBlock::SetText(const std::string& text)
{
    if (text != mText) {
        mText = text;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::string UiTextBlock::GetText() const
{
    return mText;
}

void UiTextBlock::SetOpacity(const float opacity)
{
    if (glm::abs(mOpacity - opacity) > EngineMath::ENGINE_FLOAT_EPSILON) {
        mOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

float UiTextBlock::GetOpacity() const
{
    return mOpacity;
}

std::string UiTextBlock::GetFontName() const
{
    return mFontName;
}

glm::ivec2 UiTextBlock::GetTextLineWidthHeight() const
{
    return mTextLineWidthHeight;
}

void UiTextBlock::SetFontSize(const int32_t fontSize)
{
    if (mFontSize != fontSize) {
        mFontSize = fontSize;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

int32_t UiTextBlock::GetFontSize() const
{
    return mFontSize;
}

void UiTextBlock::SetTextColor(const glm::vec3& color)
{
    if (!EngineMath::CheckSimilarityVec3(color, mTextColor)) {
        mTextColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetTextColor(const uint32_t hexColor)
{
    SetTextColor(EngineMath::FromHexColorToVec3Color(hexColor));
}

glm::vec3 UiTextBlock::GetTextColor() const
{
    return mTextColor;
}

void UiTextBlock::SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment)
{
    if (mTextHorizontalAlignment != textHorizontalAlignment) {
        mTextHorizontalAlignment = textHorizontalAlignment;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

eTextHorizontalAlignmentType UiTextBlock::GetTextHorizontalAlignment() const
{
    return mTextHorizontalAlignment;
}

void UiTextBlock::SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment)
{
    if (mTextVerticalAlignment != textVerticalAlignment) {
        mTextVerticalAlignment = textVerticalAlignment;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

eTextVerticalAlignmentType UiTextBlock::GetTextVerticalAlignment() const
{
    return mTextVerticalAlignment;
}

void UiTextBlock::SetRectangleColor(const glm::vec3& color)
{
    if (mRectangleColor != color) {
        mRectangleColor = color;
    }
}

void UiTextBlock::SetRectangleOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mRectangleOpacity, opacity)) {
        mRectangleOpacity = opacity;
    }
}

void UiTextBlock::SetRectangleRadius(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mRectangleRadius, borderRadius)) {
        mRectangleRadius = borderRadius;
    }
}

glm::vec3 UiTextBlock::GetRectangleColor() const
{
    return mRectangleColor;
}

float UiTextBlock::GetRectangleOpacity() const
{
    return mRectangleOpacity;
}

float UiTextBlock::GetRectangleRadius() const
{
    return mRectangleRadius;
}

glm::vec3 UiTextBlock::GetBorderColor() const
{
    return mBorderColor;
}

float UiTextBlock::GetBorderOpacity() const
{
    return mBorderOpacity;
}

float UiTextBlock::GetBorderRadius() const
{
    return mBorderRadius;
}

void UiTextBlock::SetBorderColor(const glm::vec3& color)
{
    if (mBorderColor != color) {
        mBorderColor = color;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetBorderRadius(const float borderRadius)
{
    if (!EngineMath::FloatsNearEqual(mBorderRadius, borderRadius)) {
        mBorderRadius = borderRadius;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetBorderOpacity(const float opacity)
{
    if (!EngineMath::FloatsNearEqual(mBorderOpacity, opacity)) {
        mBorderOpacity = opacity;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

void UiTextBlock::SetAttachTargetUiItemName(const std::string& uiItemName)
{
    if (mAttachTargetUiItemName != uiItemName) {
        mAttachTargetUiItemName = uiItemName;
        RecalculateAnchorPositions();
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

std::string UiTextBlock::GetAttachTargetUiItemName() const
{
    return mAttachTargetUiItemName;
}

void UiTextBlock::SetBorderThickness(const int32_t thickness)
{
    if (mBorderThickness != thickness) {
        mBorderThickness = thickness;
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
        SetIsPropertiesShouldBeUpdatedOnLuaThread(true);
    }
}

int32_t UiTextBlock::GetBorderThickness() const
{
    return mBorderThickness;
}

std::shared_ptr<UiSceneProxyBase> UiTextBlock::CreateUiSceneProxy() const
{
    return std::make_shared<UiTextBlockSceneProxy>(this);
}

std::shared_ptr<LuaProxy> UiTextBlock::ReplicateLuaProxy()
{
    return std::make_shared<UiTextBlockLuaProxy>(std::static_pointer_cast<UiTextBlock>(shared_from_this()));
}

void UiTextBlock::RecalculateAnchorPositions()
{
    if (mAttachTargetUiItemName.empty()) {
        UiItemBase::RecalculateAnchorPositions();
        return;
    }
    // If attach target is set, recalculate absolute position according to it
    RecalculatePositionAccordingToAttachTarget();
    RebuildBoundingArea();
    RebuildNormalizedTransform();
}

void UiTextBlock::RecalculatePositionAccordingToAttachTarget()
{
    if (const auto& parentCanvasSp = mParentCanvas.lock()) {
        if (const auto targetUiItemSp = parentCanvasSp->TryFindHierarchyChildByName(mAttachTargetUiItemName)) {
            const glm::ivec2 newOrigin = FindFreeAttachPosition(targetUiItemSp->GetBoundingArea());
            SetAbsoluteOrigin(newOrigin);
        }
    }
}

glm::ivec2 UiTextBlock::FindFreeAttachPosition(const BoundingBox2D<glm::ivec2>& targetUiBoundingArea) const
{
    glm::ivec2 result(0);

    if (const auto& parentCanvasSp = mParentCanvas.lock()) {
        const glm::ivec2 spaceSize = glm::ivec2(parentCanvasSp->GetWidth(), parentCanvasSp->GetHeight());
        enum eCandidatePosition { TOP = 0, BOTTOM = 1, LEFT = 2, RIGHT = 3 };
        std::array<BoundingBox2D<glm::ivec2>, 4> candidatePositionAreas;
        const int32_t topAreaHeight = (spaceSize.y - targetUiBoundingArea.GetMax().y);
        candidatePositionAreas[eCandidatePosition::TOP] = BoundingBox2D<glm::ivec2>(
            glm::ivec2(spaceSize.x / 2, targetUiBoundingArea.GetMax().y + (topAreaHeight / 2)),
            glm::ivec2(spaceSize.x / 2, topAreaHeight / 2));
        const int32_t bottomAreaHeight = targetUiBoundingArea.GetMin().y;
        candidatePositionAreas[eCandidatePosition::BOTTOM] = BoundingBox2D<glm::ivec2>(
            glm::ivec2(spaceSize.x / 2, bottomAreaHeight / 2), glm::ivec2(spaceSize.x / 2, bottomAreaHeight / 2));
        const int32_t leftAreaWidth = targetUiBoundingArea.GetMin().x;
        candidatePositionAreas[eCandidatePosition::LEFT] = BoundingBox2D<glm::ivec2>(
            glm::ivec2(leftAreaWidth / 2, spaceSize.y / 2), glm::ivec2(leftAreaWidth / 2, spaceSize.y / 2));
        const int32_t rightAreaWidth = spaceSize.x - targetUiBoundingArea.GetMax().x;
        candidatePositionAreas[eCandidatePosition::RIGHT] = BoundingBox2D<glm::ivec2>(
            glm::ivec2(targetUiBoundingArea.GetMax().x + (rightAreaWidth / 2), spaceSize.y / 2),
            glm::ivec2(rightAreaWidth / 2, spaceSize.y / 2));

        int32_t bestAreaIndex = -1;
        for (size_t i = 0; i < candidatePositionAreas.size(); ++i) {
            const auto& candidateArea = candidatePositionAreas[i];
            if (candidateArea.GetHalfExtent().x >= GetBoundingArea().GetHalfExtent().x
                && candidateArea.GetHalfExtent().y >= GetBoundingArea().GetHalfExtent().y) {
                bestAreaIndex = static_cast<int32_t>(i);
                break; // take first found
            }
        }

        if (bestAreaIndex != -1) {
            const auto& bestArea = candidatePositionAreas[bestAreaIndex];
            if (eCandidatePosition::TOP == bestAreaIndex) {
                result = glm::ivec2(targetUiBoundingArea.GetOrigin().x, targetUiBoundingArea.GetMax().y);
            } else if (eCandidatePosition::BOTTOM == bestAreaIndex) {
                result = glm::ivec2(targetUiBoundingArea.GetOrigin().x, targetUiBoundingArea.GetMin().y - GetHeight());
            } else if (eCandidatePosition::LEFT == bestAreaIndex) {
                result = glm::ivec2(targetUiBoundingArea.GetMin().x - GetWidth(), targetUiBoundingArea.GetOrigin().y);

            } else if (eCandidatePosition::RIGHT == bestAreaIndex) {
                result = glm::ivec2(targetUiBoundingArea.GetMax().x, targetUiBoundingArea.GetOrigin().y);
            }

            if (result.x + GetWidth() > spaceSize.x) {
                result.x = spaceSize.x - GetWidth();
            }
            if (result.x < 0) {
                result.x = 0;
            }
            if (result.y + GetHeight() > spaceSize.y) {
                result.y = spaceSize.y - GetHeight();
            }
            if (result.y < 0) {
                result.y = 0;
            }
        } else {
            result = glm::ivec2(spaceSize) / 2; // center
            LogInfo("UiTextBlock::FindFreeAttachPosition: no free space found, placing in center");
        }
    }

    return result;
}

void UiTextBlock::SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr)
{
    UiItemBase::SyncFromLuaJsonProperties(luaJsonPropsStr);

    bool bShouldUpdatePropertiesOnRT = false;

    const auto jsonObj = nlohmann::json::parse(luaJsonPropsStr);
    if (jsonObj.contains("text")) {
        const auto text = jsonObj["text"].get<std::string>();
        if (mText != text) {
            mText = text;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_color")) {
        const glm::vec3 color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["text_color"]);
        if (!EngineMath::CheckSimilarityVec3(color, mTextColor)) {
            mTextColor = color;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_opacity")) {
        const auto opacity = jsonObj["text_opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mOpacity, opacity)) {
            mOpacity = opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("font_size")) {
        const auto font_size = jsonObj["font_size"].get<int32_t>();
        if (mFontSize != font_size) {
            mFontSize = font_size;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_horizontal_alignment")) {
        const auto text_horizontal_alignment
            = static_cast<eTextHorizontalAlignmentType>(jsonObj["text_horizontal_alignment"].get<uint8_t>());
        if (text_horizontal_alignment != mTextHorizontalAlignment) {
            mTextHorizontalAlignment = text_horizontal_alignment;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("text_vertical_alignment")) {
        const auto text_vertical_alignment
            = static_cast<eTextVerticalAlignmentType>(jsonObj["text_vertical_alignment"].get<uint8_t>());
        if (text_vertical_alignment != mTextVerticalAlignment) {
            mTextVerticalAlignment = text_vertical_alignment;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("rectangle_opacity")) {
        const auto rectangle_opacity = jsonObj["rectangle_opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mRectangleOpacity, rectangle_opacity)) {
            mRectangleOpacity = rectangle_opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("rectangle_color")) {
        const glm::vec3 rectangle_color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["rectangle_color"]);
        if (!EngineMath::CheckSimilarityVec3(rectangle_color, mRectangleColor)) {
            mRectangleColor = rectangle_color;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("rectangle_radius")) {
        const auto rectangle_radius = jsonObj["rectangle_radius"].get<float>();
        if (!EngineMath::FloatsNearEqual(mRectangleRadius, rectangle_radius)) {
            mRectangleRadius = rectangle_radius;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("border_color")) {
        const glm::vec3 border_color = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["border_color"]);
        if (!EngineMath::CheckSimilarityVec3(border_color, mBorderColor)) {
            mBorderColor = border_color;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("border_radius")) {
        const auto border_radius = jsonObj["border_radius"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderRadius, border_radius)) {
            mBorderRadius = border_radius;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("border_opacity")) {
        const auto border_opacity = jsonObj["border_opacity"].get<float>();
        if (!EngineMath::FloatsNearEqual(mBorderOpacity, border_opacity)) {
            mBorderOpacity = border_opacity;
            bShouldUpdatePropertiesOnRT = true;
        }
    }
    if (jsonObj.contains("attach_target_ui_item_name")) {
        const auto attach_target_ui_item_name = jsonObj["attach_target_ui_item_name"].get<std::string>();
        if (mAttachTargetUiItemName != attach_target_ui_item_name) {
            mAttachTargetUiItemName = attach_target_ui_item_name;
            RecalculateAnchorPositions();
        }
    }
    if (jsonObj.contains("border_thickness")) {
        const auto border_thickness = jsonObj["border_thickness"].get<int32_t>();
        if (mBorderThickness != border_thickness) {
            mBorderThickness = border_thickness;
            bShouldUpdatePropertiesOnRT = true;
        }
    }

    if (bShouldUpdatePropertiesOnRT) {
        SetIsPropertiesShouldBeUpdatedOnRenderThread(true);
    }
}

void UiTextBlock::SyncDataOnRenderThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiTextBlock::SyncDataOnRenderThread");
    if (mIsSceneProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& canvasSp = GetParentCanvas().lock()) {
                if (const auto& sceneRenderer = sceneSp->GetInterThreadCommunicationManager().GetSceneRendererWP().lock()) {

                    sceneSp->GetInterThreadCommunicationManager().ExecuteOnRenderThread(
                        eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                        GetUId(),
                        functionId,
                        [sceneRenderer,
                         myUId = GetUId(),
                         canvasUId = canvasSp->GetUId(),
                         opacity = mOpacity,
                         text = mText,
                         textLineWidthHeight = mTextLineWidthHeight,
                         fontSize = mFontSize,
                         textColor = mTextColor,
                         textHorizontalAlignment = mTextHorizontalAlignment,
                         textVerticalAlignment = mTextVerticalAlignment,
                         rectangleOpacity = mRectangleOpacity,
                         rectangleColor = mRectangleColor,
                         rectangleBorderRadius = mRectangleRadius,
                         borderColor = mBorderColor,
                         borderRadius = mBorderRadius,
                         borderOpacity = mBorderOpacity,
                         borderThickness = mBorderThickness](
                            std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                            std::weak_ptr<EngineCore::Scene> sceneWp,
                            std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                            const auto& uiSceneProxy = sceneRenderer->GetUiSceneProxyByProxyId(myUId, canvasUId);
                            if (uiSceneProxy) {
                                const auto& textBlockSceneProxy = std::static_pointer_cast<UiTextBlockSceneProxy>(uiSceneProxy);
                                textBlockSceneProxy->SetOpacity(opacity);
                                textBlockSceneProxy->SetText(text);
                                textBlockSceneProxy->SetTextLineWidthHeight(textLineWidthHeight);
                                textBlockSceneProxy->SetFontSize(fontSize);
                                textBlockSceneProxy->SetTextColor(textColor);
                                textBlockSceneProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                                textBlockSceneProxy->SetTextVerticalAlignment(textVerticalAlignment);
                                textBlockSceneProxy->SetRectangleOpacity(rectangleOpacity);
                                textBlockSceneProxy->SetRectangleColor(rectangleColor);
                                textBlockSceneProxy->SetRectangleRadius(rectangleBorderRadius);
                                textBlockSceneProxy->SetBorderColor(borderColor);
                                textBlockSceneProxy->SetBorderRadius(borderRadius);
                                textBlockSceneProxy->SetBorderOpacity(borderOpacity);
                                textBlockSceneProxy->SetBorderThickness(borderThickness);
                            }
                        });
                }
            }
        }
    } else {
        mIsPropertiesShouldBeUpdatedOnRenderThread = true;
    }
}

void UiTextBlock::SyncDataOnLuaThread()
{
    static constexpr uint64_t functionId = Hash64_CT("UiTextBlock::SyncDataOnLuaThread");
    if (mIsLuaProxyReady.load(std::memory_order::seq_cst)) {
        if (const auto& sceneSp = GetScene().lock()) {
            if (const auto& luaScriptProcessorSp = GetLuaScriptProcessorWp().lock()) {
                SetIsPropertiesShouldBeUpdatedOnLuaThread(false);
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
                    eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                    GetUId(),
                    functionId,
                    [luaScriptProcessorSp,
                     luaProxyId = GetLuaProxyId(),
                     opacity = mOpacity,
                     text = mText,
                     textColor = mTextColor,
                     textLineWidthHeight = mTextLineWidthHeight,
                     fontSize = mFontSize,
                     textHorizontalAlignment = mTextHorizontalAlignment,
                     textVerticalAlignment = mTextVerticalAlignment,
                     rectangleColor = mRectangleColor,
                     rectangleOpacity = mRectangleOpacity,
                     rectangleBorderRadius = mRectangleRadius,
                     borderColor = mBorderColor,
                     borderRadius = mBorderRadius,
                     borderOpacity = mBorderOpacity,
                     attachTargetUiItemName = mAttachTargetUiItemName,
                     borderThickness = mBorderThickness](
                        std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                        std::weak_ptr<EngineCore::Scene> sceneWp,
                        std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                        if (const auto& textBlockLuaProxy
                            = std::static_pointer_cast<UiTextBlockLuaProxy>(luaScriptProcessorSp->GetLuaProxy(luaProxyId))) {
                            textBlockLuaProxy->SetOpacity_FromGameThread(opacity);
                            textBlockLuaProxy->SetText_FromGameThread(text);
                            textBlockLuaProxy->SetTextColor_FromGameThread(textColor);
                            textBlockLuaProxy->SetTextLineWidthHeight_FromGameThread(textLineWidthHeight);
                            textBlockLuaProxy->SetFontSize_FromGameThread(fontSize);
                            textBlockLuaProxy->SetTextHorizontalAlignment(textHorizontalAlignment);
                            textBlockLuaProxy->SetTextVerticalAlignment(textVerticalAlignment);
                            textBlockLuaProxy->SetRectangleColor_FromGameThread(rectangleColor);
                            textBlockLuaProxy->SetRectangleOpacity_FromGameThread(rectangleOpacity);
                            textBlockLuaProxy->SetRectangleBorderRadius_FromGameThread(rectangleBorderRadius);
                            textBlockLuaProxy->SetBorderColor_FromGameThread(borderColor);
                            textBlockLuaProxy->SetBorderRadius_FromGameThread(borderRadius);
                            textBlockLuaProxy->SetBorderOpacity_FromGameThread(borderOpacity);
                            textBlockLuaProxy->SetAttachTargetUiItemName_FromGameThread(attachTargetUiItemName);
                            textBlockLuaProxy->SetBorderThickness_FromGameThread(borderThickness);
                        }
                    });
            }
        }
    }
}
} // namespace GUI
} // namespace EngineCore