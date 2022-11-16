#include "UiImage.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/GraphicsCore/Common/ScreenQuad.h"

using namespace Resources;
using namespace IO;
using namespace Graphics;

namespace EngineCore
{
    namespace GUI
    {
        UiImage::UiImage(const std::weak_ptr<IUiTransformable> &parent)
            : UiItemBase(parent)
        {
            const auto &folderManager = FolderManager::GetInstance();
            ShaderParams shaderParams("UiTest Shader", folderManager->GetShadersPath() + "uiTestVS.glsl", folderManager->GetShadersPath() + "uiTestFS.glsl", "", "", "", "");
            mUiTestShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiTestShader>(shaderParams);
        }

        void UiImage::Render()
        {
            const auto &absoluteTranslation = mAbsoluteOrigin.Translation;
            glViewport(absoluteTranslation.x, absoluteTranslation.y, mWidth, mHeight);
            mUiTestShader->ExecuteShader();
            mUiTestShader->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiTestShader->StopShader();
        }

        void UiImage::UpdateHierarchyTransform()
        {
            UiItemBase::UpdateHierarchyTransform();
        }
    }
}