#pragma once

#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GameCore/ShaderImplementation/WaterPlaneShader.h"

using namespace Graphics::Mesh;
using namespace Graphics::Texture;
using namespace Game::ShaderImpl;

namespace Graphics
{

   namespace Data
   {
      struct WaterPlaneRenderData
      {

         std::shared_ptr<Skin> m_skin;

         std::shared_ptr<ICompositeShader> m_materialShader;

         WaterPlaneRenderData(std::shared_ptr<Skin> skin, std::shared_ptr<IShader> materialShader)
            : m_skin(skin)
            , m_materialShader(std::dynamic_pointer_cast<ICompositeShader>(materialShader))

         {
         }
         
      };
   }

}
