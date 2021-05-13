#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GraphicsCore/Material/MaterialProxy.h"

#include <memory>

namespace Graphics
{
   namespace OpenGL
   {
      struct CompositeShaderParams
      {

         std::shared_ptr<Shader> mShader;
         std::string mShaderName;

         const uint64_t HASH;

         CompositeShaderParams(const uint64_t hash, const std::string& shaderName, const std::shared_ptr<Shader>& shader)
            : mShader(shader)
            , mShaderName(shaderName)
            , HASH(hash)
         {
         }

         bool operator==(const CompositeShaderParams& other) const {

            return this->mShaderName == other.mShaderName
               && this->HASH == other.HASH;
         }
      };

      struct CompositeMaterialShaderParams
         : public CompositeShaderParams
      {

         std::shared_ptr<MaterialProxy> mMaterialProxy;

         CompositeMaterialShaderParams(const uint64_t hash, const std::string& shaderName, const std::shared_ptr<Shader>& shader, std::shared_ptr<MaterialProxy> materialProxy)
            : CompositeShaderParams(hash, shaderName, shader)
            , mMaterialProxy(materialProxy)
         {
         }
      };

#define NAME_TO_STR(name) #name
#define COMPOSITE_MATERIAL_SHADER_TO_STR(name1, name2, materialName) (NAME_TO_STR(name1) ## NAME_TO_STR(name2) + materialName)
#define COMPOSITE_SHADER_TO_STR(name1, name2) (NAME_TO_STR(name1) ## NAME_TO_STR(name2))

      template <typename CompositeShaderType>
      struct TemplatedCompositeMaterialShaderParams
         : public CompositeMaterialShaderParams
      {

         TemplatedCompositeMaterialShaderParams(const std::string& uniqueName, const ShaderParams& shaderParams, std::shared_ptr<MaterialProxy> materialProxy)
            : CompositeMaterialShaderParams(
               Game::Hash(uniqueName),
               uniqueName,
               std::make_shared<typename CompositeShaderType::shader_t>(shaderParams),
               materialProxy)
         {

         }
      };

      template <typename CompositeShaderType>
      struct TemplatedCompositeShaderParams
         : public CompositeShaderParams
      {

         TemplatedCompositeShaderParams(const std::string& uniqueName, const ShaderParams& shaderParams)
            : CompositeShaderParams(
               Game::Hash(uniqueName),
               uniqueName,
               std::make_shared<typename CompositeShaderType::shader_t>(shaderParams))
         {

         }
      };
   }
}

namespace std
{
   using namespace Graphics::OpenGL;

   template<>
   struct hash<CompositeShaderParams>
   {
      std::uint64_t operator()(const CompositeShaderParams& k) const
      {
         return k.HASH;
      }
   };
}


