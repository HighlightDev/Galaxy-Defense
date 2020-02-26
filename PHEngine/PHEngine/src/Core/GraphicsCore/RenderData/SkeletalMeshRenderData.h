#pragma once

#include <memory>
#include <vector>

#include "Core/GraphicsCore/Mesh/Skin.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Animation/AnimationSequence.h"
#include "Core/GraphicsCore/OpenGL/Shader/CompositeShader.h"

using namespace Graphics::Mesh;
using namespace Graphics::OpenGL;
using namespace Graphics::Texture;
using namespace Graphics::Animation;

namespace Graphics
{
   namespace Data
   {
      struct SkeletalMeshRenderData
      {
         std::shared_ptr<Skin> m_skin;
         std::shared_ptr<std::vector<AnimationSequence>> m_animations;

         std::shared_ptr<ICompositeShader> m_materialShader;

         SkeletalMeshRenderData(std::shared_ptr<Skin> mesh, std::shared_ptr<std::vector<AnimationSequence>> animations, std::shared_ptr<IShader> materialShader)
            : m_skin(mesh)
            , m_animations(animations)
            , m_materialShader(std::dynamic_pointer_cast<ICompositeShader>(materialShader))
         {
         }

         ~SkeletalMeshRenderData() { }
      };
   }
}
