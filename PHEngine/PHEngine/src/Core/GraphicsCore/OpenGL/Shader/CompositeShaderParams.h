#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/CommonApi/StringHash.h"
#include "Core/GraphicsCore/Material/IMaterial.h"

#include <memory>

namespace Graphics
{
   namespace OpenGL
   {
      struct CompositeShaderParams
      {

         std::shared_ptr<ShaderBase> mShader;
         std::string mShaderName;
         std::shared_ptr<IMaterial> mMaterial;

         const uint64_t HASH;

         CompositeShaderParams(const uint64_t hash, const std::string& shaderName, const std::shared_ptr<ShaderBase>& shader, std::shared_ptr<IMaterial> materialInstance)
            : mShader(shader)
            , mShaderName(shaderName)
            , mMaterial(materialInstance)
            , HASH(hash)
         {
         }

         bool operator==(const CompositeShaderParams& other) const {

            return this->mShaderName == other.mShaderName
               && this->HASH == other.HASH;
         }
      };

#define NAME_TO_STR(name) #name
#define COMPOSITE_SHADER_TO_STR(name1, name2, materialName) (NAME_TO_STR(name1) ## NAME_TO_STR(name2) + materialName)

      template <typename CompositeShaderType>
      struct TemplatedCompositeShaderParams 
         : public CompositeShaderParams
      {

         TemplatedCompositeShaderParams(const std::string& uniqueName, const ShaderParams& shaderParams, std::shared_ptr<IMaterial> materialInstance)
            : CompositeShaderParams(
               Game::Hash(uniqueName),
               uniqueName,
               std::make_shared<typename CompositeShaderType::shader_t>(shaderParams),
               materialInstance)
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


