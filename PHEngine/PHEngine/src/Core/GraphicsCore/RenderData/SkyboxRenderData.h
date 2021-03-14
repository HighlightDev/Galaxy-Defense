#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"

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
         std::shared_ptr<ICompositeShader> m_planarReflectionShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         SkyboxRenderData(std::shared_ptr<Skin> skyboxMesh,
            std::shared_ptr<IShader> materialShader,
            std::shared_ptr<IShader> planarReflectionShader,
            std::shared_ptr<MaterialProxy> materialProxy)
            : m_skin(skyboxMesh)
            , m_materialShader(std::dynamic_pointer_cast<ICompositeShader>(materialShader))
            , m_planarReflectionShader(std::dynamic_pointer_cast<ICompositeShader>(planarReflectionShader))
            , mMaterialProxy(materialProxy)
         {
         }

         ~SkyboxRenderData()
         {
         }
      };
   }
}
