#pragma once

#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "FxColorResolver/FxColorResolver.h"
#include "IPostFxPass.h"
#include "IPostFxRenderTargetProvider.h"

#include <bitset>
#include <memory>
#include <unordered_map>

namespace Graphics {

class ResolvedSceneFramebuffer;

class PostFxRenderer : public IPostFxRenderTargetProvider {
private:
    std::bitset<4> mEnabledFxBits;

    ViewPortInfo mViewPortInfo;

    std::unordered_map<ePostFxStageType, std::shared_ptr<IPostFxPass>> mPostFxStages;

    std::unique_ptr<FxColorResolver> mFxColorResolver;

    std::unordered_map<ePostFxStageType, std::shared_ptr<ITexture>> mPostFxResultCache;

public:
    PostFxRenderer(const ViewPortInfo& viewPortInfo);

    ~PostFxRenderer() override;

    std::shared_ptr<ITexture> GetRenderTargetTextureByKey(const ePostFxStageType postFxStageType) const override;

    void ExecuteResolveSceneColor(const std::shared_ptr<ResolvedSceneFramebuffer>& resolveSceneColorFramebuffer);

    void ExecuteResolveGuiColor(const std::shared_ptr<ResolvedSceneFramebuffer>& resolveUiFramebuffer);

    void ResizeRenderTargets(const ViewPortInfo& viewPortInfo);

private:
    void Init();
};

} // namespace Graphics
