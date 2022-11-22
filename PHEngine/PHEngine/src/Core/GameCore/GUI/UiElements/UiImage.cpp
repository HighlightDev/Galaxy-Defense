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
            : UiItemBase(parent),
              mColor(0.0, 0.0, 0.0, 1.0)
        {
            const auto &folderManager = FolderManager::GetInstance();
            ShaderParams shaderParams("UiTest Shader", folderManager->GetShadersPath() + "uiTestVS.glsl", folderManager->GetShadersPath() + "uiTestFS.glsl", "", "", "", "");
            mUiTestShader = ShaderPool::GetInstance()->template GetOrAllocateResource<UiTestShader>(shaderParams);
        }

        void UiImage::Render()
        {
            const auto &transformMatrix = GetTransformMatrix();
            mUiTestShader->ExecuteShader();
            mUiTestShader->SetTransformMatrix(transformMatrix);
            mUiTestShader->SetColor(mColor);
            ScreenQuad::GetInstance()->GetBuffer()->RenderVAO(GL_TRIANGLES);
            mUiTestShader->StopShader();
        }

        void UiImage::UpdateHierarchyTransform()
        {
            UiItemBase::UpdateHierarchyTransform();
        }

        void UiImage::SetColor(const glm::vec4 &color)
        {
            mColor = color;
        }

        glm::vec4 UiImage::GetColor() const
        {
            return mColor;
        }
    }
}