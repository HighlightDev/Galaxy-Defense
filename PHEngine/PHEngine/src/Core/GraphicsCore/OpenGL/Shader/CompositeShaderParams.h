#pragma once

#include "Core/CommonCore/StringHash.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderParams.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"

#include <memory>

namespace Graphics
{
   namespace OpenGL
   {
      struct CompositeShaderParams
      {
         ShaderParams mShaderParams;
         std::string mShaderName;

         const uint64_t HASH;

         CompositeShaderParams(const uint64_t hash, const std::string &shaderName, const ShaderParams &shaderParams)
             : mShaderParams(shaderParams),
               mShaderName(shaderName),
               HASH(hash)
         {
         }

         bool operator==(const CompositeShaderParams &other) const
         {

            return this->mShaderName == other.mShaderName && this->HASH == other.HASH;
         }
      };

      struct CompositeMaterialShaderParams
          : public CompositeShaderParams
      {

         std::shared_ptr<MaterialProxy> mMaterialProxy;

         CompositeMaterialShaderParams(const uint64_t hash,
                                       const std::string &shaderName,
                                       const ShaderParams &shaderParams,
                                       std::shared_ptr<MaterialProxy> materialProxy)
             : CompositeShaderParams(hash, shaderName, shaderParams), mMaterialProxy(materialProxy)
         {
         }
      };

      struct TemplatedCompositeMaterialShaderParams
          : public CompositeMaterialShaderParams
      {

         TemplatedCompositeMaterialShaderParams(const std::string &uniqueName, const ShaderParams &shaderParams, std::shared_ptr<MaterialProxy> materialProxy)
             : CompositeMaterialShaderParams(
                   EngineCore::Hash(uniqueName),
                   uniqueName,
                   shaderParams,
                   materialProxy)
         {
         }
      };

      struct TemplatedCompositeShaderParams
          : public CompositeShaderParams
      {

         TemplatedCompositeShaderParams(const std::string &uniqueName, const ShaderParams &shaderParams)
             : CompositeShaderParams(
                   EngineCore::Hash(uniqueName),
                   uniqueName,
                   shaderParams)
         {
         }
      };
   }
}

namespace std
{
   using namespace Graphics::OpenGL;

   template <>
   struct hash<CompositeShaderParams>
   {
      std::uint64_t operator()(const CompositeShaderParams &k) const
      {
         return k.HASH;
      }
   };
}
