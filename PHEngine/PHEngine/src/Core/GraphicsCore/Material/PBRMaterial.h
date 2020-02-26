#pragma once

#include "IMaterial.h"
#include "MaterialProperty.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <memory>
#include <array>

using namespace Graphics::Texture;

namespace Graphics
{

   struct PBRMaterial :
      public IMaterial
   {
      using ITextureShared = std::shared_ptr<ITexture>;

      PBRMaterial(ITextureShared albedo,
         ITextureShared normalMap,
         ITextureShared metallicMap,
         ITextureShared roughnessMap,
         ITextureShared ambientOcclusionMap);

      PBRMaterial(const PBRMaterial&) = default;
      PBRMaterial& operator=(const PBRMaterial&) = default;

      virtual ~PBRMaterial();
   };

}

