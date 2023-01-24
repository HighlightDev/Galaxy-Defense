#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"

using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {
      struct SkeletalMeshRenderData
      {
         std::string mModelName;

         std::shared_ptr<IShader> m_materialShader;
         std::shared_ptr<IShader> m_planarReflectionShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;

         SkeletalMeshRenderData(const std::string &modelName,
                                std::shared_ptr<IShader> materialShader,
                                std::shared_ptr<IShader> planarReflectionShader,
                                std::shared_ptr<MaterialProxy> materialProxy)
             : mModelName(modelName),
               m_materialShader(materialShader),
               m_planarReflectionShader(planarReflectionShader),
               mMaterialProxy(materialProxy)
         {
         }

         ~SkeletalMeshRenderData() {}
      };
   }
}
