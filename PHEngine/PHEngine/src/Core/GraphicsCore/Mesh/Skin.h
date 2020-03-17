#pragma once

#include <memory>
#include <utility>

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"

using namespace Graphics::OpenGL;

namespace Graphics
{
   namespace Mesh
   {

      class Skin
      {

         VertexArrayObject m_buffer;

      public:

         Skin(const VertexArrayObject& vao);

         virtual ~Skin();

         inline VertexArrayObject* GetBuffer() {

            return &m_buffer;
         }

         virtual void CleanUp();
      };

   }
}

