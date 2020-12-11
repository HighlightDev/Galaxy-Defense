#pragma once

#include <memory>
#include <utility>

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GameCore/BoundingBox.h"

using namespace Graphics::OpenGL;
using namespace Game;

namespace Graphics
{
   namespace Mesh
   {

      class Skin
      {

         VertexArrayObject m_buffer;

         BoundingBox mBoundingBox;

      public:

         Skin(const VertexArrayObject& vao, const BoundingBox& boundingBox);

         virtual ~Skin();

         VertexArrayObject* GetBuffer();

         BoundingBox GetBoundingBox() const;

         virtual void CleanUp();
      };

   }
}

