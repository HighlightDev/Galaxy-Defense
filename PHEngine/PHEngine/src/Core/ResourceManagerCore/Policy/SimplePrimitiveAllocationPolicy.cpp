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
         std::unique_ptr<VertexArrayObject> vao = std::make_unique<VertexArrayObject>();

         std::shared_ptr<std::vector<float>> vertices;
         std::shared_ptr<std::vector<float>> normals;
         std::shared_ptr<std::vector<float>> texCoords;

         switch (typeArg)
         {
            case SimplePrimitiveType::POINT:
               vertices = std::make_shared<std::vector<float>>(std::vector<float>({ 0.0f, 0.0f, 0.0f }));
               break;
            case SimplePrimitiveType::CUBE:
               vertices = std::make_shared<std::vector<float>>(std::vector<float>({
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
                  }));
               break;
            case SimplePrimitiveType::PLANE:
            case SimplePrimitiveType::PLANE_WITH_ATTRIBUTES:
               vertices = std::make_shared<std::vector<float>>(std::vector<float>({
                  -1.0f, -1.0f, -1.0f, // top-right
                  1.0f, -1.0f, -1.0f, // top-left
                  1.0f, -1.0f, 1.0f, // bottom-left
                  1.0f, -1.0f, 1.0f, // bottom-left
                  -1.0f, -1.0f, 1.0f, // bottom-right
                  -1.0f, -1.0f, -1.0f, // top-right
                  }));
               break;
            default:
               vertices = std::make_shared<std::vector<float>>();
               break;
         }

         if (SimplePrimitiveType::PLANE_WITH_ATTRIBUTES == typeArg)
         {
            normals = std::make_shared<std::vector<float>>(std::vector<float>({
                  0.0f, 0.0f, 1.0f, // top-right
                  0.0f, 0.0f, 1.0f, // top-left
                  0.0f, 0.0f, 1.0f, // bottom-left
                  0.0f, 0.0f, 1.0f, // bottom-left
                  0.0f, 0.0f, 1.0f, // bottom-right
                  0.0f, 0.0f, 1.0f, // top-right
               }));
            texCoords = std::make_shared<std::vector<float>>(std::vector<float>({
                  1.0f, 0.0f, // top-right
                  0.0f, 0.0f, // top-left
                  0.0f, 1.0f, // bottom-left
                  0.0f, 1.0f, // bottom-left
                  1.0f, 1.0f, // bottom-right
                  1.0f, 0.0f, // top-right
               }));
         }

         VertexBufferObjectBase* vertexVBO = nullptr, *normalVBO = nullptr, *texCoordsVBO = nullptr;

         vertexVBO = new VertexBufferObject<float, 3, GL_FLOAT>(vertices, GL_ARRAY_BUFFER, 0, DataCarryFlag::Invalidate);

         if (normals && texCoords)
         {
            normalVBO = new VertexBufferObject<float, 3, GL_FLOAT>(normals, GL_ARRAY_BUFFER, 1, DataCarryFlag::Invalidate);
            texCoordsVBO = new VertexBufferObject<float, 2, GL_FLOAT>(texCoords, GL_ARRAY_BUFFER, 2, DataCarryFlag::Invalidate);
         }

         vao->AddVBO(std::move(vertexVBO), std::move(normalVBO), std::move(texCoordsVBO));

         vao->BindBuffersToVao();

         resultSkin = std::make_shared<Skin>(std::move(vao));
      }

      return resultSkin;
   }

   template <typename Model>
   void SimplePrimitiveAllocationPolicy<Model>::DeallocateMemory(const std::shared_ptr<Skin>& arg)
   {
      arg->CleanUp();
   }

}
