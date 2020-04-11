#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {

      struct SkyboxRenderData
      {

         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<ICompositeShader> m_materialShader;
         std::shared_ptr<IMaterial> mMaterialInstance;

         SkyboxRenderData(std::shared_ptr<Skin> skyboxMesh,
            std::shared_ptr<IShader> materialShader,
            std::shared_ptr<IMaterial> materialInstance)
            : m_skin(skyboxMesh)
            , m_materialShader(std::dynamic_pointer_cast<ICompositeShader>(materialShader))
            , mMaterialInstance(materialInstance)
         {
         }

         ~SkyboxRenderData()
         {
         }
      };
   }
}
