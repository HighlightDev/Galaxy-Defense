#pragma once

#include <memory>
#include <utility>

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GameCore/BoundingBox3D.h"

using namespace Graphics::OpenGL;
using namespace EngineCore;

namespace Graphics
{
   namespace Mesh
   {
      class Skin
      {

         std::shared_ptr<VertexArrayObject> m_buffer;

         BoundingBox3D mBoundingBox;

         std::string mMeshName;

      public:

         Skin(const std::shared_ptr<VertexArrayObject>& vao, const BoundingBox3D& boundingBox, const std::string& meshName);

         virtual ~Skin();

         bool operator==(const Skin& right) const;

         const std::shared_ptr<VertexArrayObject>& GetBuffer() const;

         BoundingBox3D GetBoundingBox() const;

         std::string GetMeshName() const;

         virtual void CleanUp();
      };

   }
}

