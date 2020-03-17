#include "SimplePrimitiveAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"

#include <gl/glew.h>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;

namespace Resources
{

   template class SimplePrimitiveAllocationPolicy<int32_t>;

   template <typename Model>
   std::shared_ptr<Skin> SimplePrimitiveAllocationPolicy<Model>::AllocateMemory(int32_t arg)
   {
      auto typeArg = SimplePrimitiveType(arg);

      std::shared_ptr<Skin> resultSkin;

      {
         VertexArrayObject vao;

         std::vector<float> vertices;
         std::vector<float> normals;
         std::vector<float> texCoords;

         switch (typeArg)
         {
            case SimplePrimitiveType::POINT:
               vertices = std::vector<float>({ 0.0f, 0.0f, 0.0f });
               break;
            case SimplePrimitiveType::CUBE:
               vertices = std::vector<float>({
                  // back face
                  -1.0f, -1.0f, -1.0f, // bottom-left
                   1.0f,  1.0f, -1.0f, // top-right
                   1.0f, -1.0f, -1.0f, // bottom-right         
                   1.0f,  1.0f, -1.0f, // top-right
                  -1.0f, -1.0f, -1.0f, // bottom-left
                  -1.0f,  1.0f, -1.0f, // top-left
                  // front face
                  -1.0f, -1.0f,  1.0f, // bottom-left
                   1.0f, -1.0f,  1.0f, // bottom-right
                   1.0f,  1.0f,  1.0f, // top-right
                   1.0f,  1.0f,  1.0f, // top-right
                  -1.0f,  1.0f,  1.0f, // top-left
                  -1.0f, -1.0f,  1.0f, // bottom-left
                  // left face
                  -1.0f,  1.0f,  1.0f, // top-right
                  -1.0f,  1.0f, -1.0f, // top-left
                  -1.0f, -1.0f, -1.0f, // bottom-left
                  -1.0f, -1.0f, -1.0f, // bottom-left
                  -1.0f, -1.0f,  1.0f, // bottom-right
                  -1.0f,  1.0f,  1.0f, // top-right
                  // right face
                   1.0f,  1.0f,  1.0f, // top-left
                   1.0f, -1.0f, -1.0f, // bottom-right
                   1.0f,  1.0f, -1.0f, // top-right         
                   1.0f, -1.0f, -1.0f, // bottom-right
                   1.0f,  1.0f,  1.0f, // top-left
                   1.0f, -1.0f,  1.0f, // bottom-left     
                  // bottom face
                  -1.0f, -1.0f, -1.0f, // top-right
                   1.0f, -1.0f, -1.0f, // top-left
                   1.0f, -1.0f,  1.0f, // bottom-left
                   1.0f, -1.0f,  1.0f, // bottom-left
                  -1.0f, -1.0f,  1.0f, // bottom-right
                  -1.0f, -1.0f, -1.0f, // top-right
                  // top face
                  -1.0f,  1.0f, -1.0f, // top-left
                   1.0f,  1.0f , 1.0f, // bottom-right
                   1.0f,  1.0f, -1.0f, // top-right     
                   1.0f,  1.0f,  1.0f, // bottom-right
                  -1.0f,  1.0f, -1.0f, // top-left
                  -1.0f,  1.0f,  1.0f // bottom-left        
                  });
               break;
            case SimplePrimitiveType::PLANE:
            case SimplePrimitiveType::PLANE_WITH_ATTRIBUTES:
               vertices = std::vector<float>({
                  -1.0f, -1.0f, -1.0f, // top-right
                  1.0f, -1.0f, -1.0f, // top-left
                  1.0f, -1.0f, 1.0f, // bottom-left
                  1.0f, -1.0f, 1.0f, // bottom-left
                  -1.0f, -1.0f, 1.0f, // bottom-right
                  -1.0f, -1.0f, -1.0f, // top-right
                  });
               break;
         }

         if (SimplePrimitiveType::PLANE_WITH_ATTRIBUTES == typeArg)
         {
            normals = std::vector<float>({
                  0.0f, 0.0f, 1.0f, // top-right
                  0.0f, 0.0f, 1.0f, // top-left
                  0.0f, 0.0f, 1.0f, // bottom-left
                  0.0f, 0.0f, 1.0f, // bottom-left
                  0.0f, 0.0f, 1.0f, // bottom-right
                  0.0f, 0.0f, 1.0f, // top-right
               });
            texCoords = std::vector<float>({
                  1.0f, 0.0f, // top-right
                  0.0f, 0.0f, // top-left
                  0.0f, 1.0f, // bottom-left
                  0.0f, 1.0f, // bottom-left
                  1.0f, 1.0f, // bottom-right
                  1.0f, 0.0f, // top-right
               });
         }

         VertexBufferObjectBase* vertexVBO = nullptr, *normalVBO = nullptr, *texCoordsVBO = nullptr;

         vertexVBO = new VertexBufferObject<float, 3, GL_FLOAT>(vertices, GL_ARRAY_BUFFER, 0, DataCarryFlag::Invalidate);

         if (normals.size() > 0 && texCoords.size() > 0)
         {
            normalVBO = new VertexBufferObject<float, 3, GL_FLOAT>(normals, GL_ARRAY_BUFFER, 1, DataCarryFlag::Invalidate);
            texCoordsVBO = new VertexBufferObject<float, 2, GL_FLOAT>(texCoords, GL_ARRAY_BUFFER, 2, DataCarryFlag::Invalidate);
         }

         vao.AddVBO(vertexVBO, normalVBO, texCoordsVBO);

         vao.BindBuffersToVao();

         resultSkin = std::make_shared<Skin>(vao);
      }

      return resultSkin;
   }

   template <typename Model>
   void SimplePrimitiveAllocationPolicy<Model>::DeallocateMemory(const std::shared_ptr<Skin>& arg)
   {
      arg->CleanUp();
   }

}
