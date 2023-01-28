#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;

namespace Graphics
{
   namespace Data
   {
      struct StaticMeshRenderData
      {
         std::string mModelPath;
         std::shared_ptr<IShader> m_materialShader;
         std::shared_ptr<IShader> m_planarReflectionShader;
         std::shared_ptr<MaterialProxy> mMaterialProxy;
         bool mIsDeferredShaded;

         StaticMeshRenderData(
             const std::string modelPath,
             std::shared_ptr<IShader> materialShader,
             std::shared_ptr<IShader> planarReflectionShader,
             std::shared_ptr<MaterialProxy> materialProxy,
             const bool isDeferredShaded)
             : mModelPath(modelPath),
               m_materialShader(materialShader),
               m_planarReflectionShader(planarReflectionShader),
               mMaterialProxy(materialProxy),
               mIsDeferredShaded(isDeferredShaded)
         {
         }

         virtual ~StaticMeshRenderData() {}
      };
   }
}
