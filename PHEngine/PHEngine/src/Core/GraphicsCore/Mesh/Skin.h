#pragma once

#include <memory>
#include <utility>

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GameCore/BoundingBox.h"

using namespace Graphics::OpenGL;
using namespace EngineCore;

namespace Graphics
{
   namespace Mesh
   {

      class Skin
      {

         std::shared_ptr<VertexArrayObject> m_buffer;

         BoundingBox mBoundingBox;

      public:

         Skin(const std::shared_ptr<VertexArrayObject>& vao, const BoundingBox& boundingBox);

         virtual ~Skin();

         const std::shared_ptr<VertexArrayObject>& GetBuffer() const;

         BoundingBox GetBoundingBox() const;

         virtual void CleanUp();
      };

   }
}

