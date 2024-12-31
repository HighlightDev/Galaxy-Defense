#include "SimplePrimitiveAllocationPolicy.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/IndexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/eAttribArrayIndex.h"
#include "Core/GameCore/BoundingBoxBuilder.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/ResourceManagerCore/SimpleMeshType.h"

#include <gl/glew.h>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources
{
   std::shared_ptr<Skin> SimplePrimitiveAllocationPolicy::AllocateMemory(const SimplePrimitivePoolParameters &arg)
   {
      LogInfo("SimplePrimitiveAllocationPolicy::AllocateMemory: ", static_cast<int32_t>(arg.mSimplePrimitiveType));
      const auto typeMesh = arg.mSimplePrimitiveType;

      std::shared_ptr<Skin> resultSkin;

      {
         const auto vao = std::make_shared<VertexArrayObject>();

         std::vector<float> vertices;
         std::vector<float> normals;
         std::vector<float> texCoords;

         switch (typeMesh)
         {
         case SimplePrimitiveType::POINT:
            vertices = std::vector<float>({0.0f, 0.0f, 0.0f});
            break;
         case SimplePrimitiveType::INVERTED_VERTICES_DIRECTION_CUBE:
            vertices = std::vector<float>({
                // back face
                -1.0f, -1.0f, -1.0f, // bottom-left
                1.0f, -1.0f, -1.0f,  // bottom-right
                1.0f, 1.0f, -1.0f,   // top-right
                1.0f, 1.0f, -1.0f,   // top-right
                -1.0f, 1.0f, -1.0f,  // top-left
                -1.0f, -1.0f, -1.0f, // bottom-left
                // front face
                -1.0f, -1.0f, 1.0f, // bottom-left
                1.0f, 1.0f, 1.0f,   // top-right
                1.0f, -1.0f, 1.0f,  // bottom-right
                1.0f, 1.0f, 1.0f,   // top-right
                -1.0f, -1.0f, 1.0f, // bottom-left
                -1.0f, 1.0f, 1.0f,  // top-left
                // left face
                -1.0f, 1.0f, 1.0f,   // top-right
                -1.0f, -1.0f, -1.0f, // bottom-left
                -1.0f, 1.0f, -1.0f,  // top-left
                -1.0f, -1.0f, -1.0f, // bottom-left
                -1.0f, 1.0f, 1.0f,   // top-right
                -1.0f, -1.0f, 1.0f,  // bottom-right
                                     // right face
                1.0f, 1.0f, 1.0f,    // top-left
                1.0f, 1.0f, -1.0f,   // top-right
                1.0f, -1.0f, -1.0f,  // bottom-right
                1.0f, -1.0f, -1.0f,  // bottom-right
                1.0f, -1.0f, 1.0f,   // bottom-left
                1.0f, 1.0f, 1.0f,    // top-left
                // bottom face
                -1.0f, -1.0f, -1.0f, // top-right
                1.0f, -1.0f, 1.0f,   // bottom-left
                1.0f, -1.0f, -1.0f,  // top-left
                1.0f, -1.0f, 1.0f,   // bottom-left
                -1.0f, -1.0f, -1.0f, // top-right
                -1.0f, -1.0f, 1.0f,  // bottom-right
                // top face
                -1.0f, 1.0f, -1.0f, // top-left
                1.0f, 1.0f, -1.0f,  // top-right
                1.0f, 1.0f, 1.0f,   // bottom-right
                1.0f, 1.0f, 1.0f,   // bottom-right
                -1.0f, 1.0f, 1.0f,  // bottom-left
                -1.0f, 1.0f, -1.0f  // top-left
            });
            break;
         case SimplePrimitiveType::QUAD:
            vertices = std::vector<float>({
                -1.0f, -1.0f, 0.0f, // bottom-left
                1.0f, -1.0f, 0.0f,  // bottom-right
                1.0f, 1.0f, 0.0f,   // top-right
                1.0f, 1.0f, 0.0f,   // top-right
                -1.0f, 1.0f, 0.0f,  // top-left
                -1.0f, -1.0f, 0.0f, // bottom-left
            });
            break;
         case SimplePrimitiveType::CUBE:
            vertices = std::vector<float>({
                // back face
                -1.0f, -1.0f, -1.0f, // bottom-left
                1.0f, 1.0f, -1.0f,   // top-right
                1.0f, -1.0f, -1.0f,  // bottom-right
                1.0f, 1.0f, -1.0f,   // top-right
                -1.0f, -1.0f, -1.0f, // bottom-left
                -1.0f, 1.0f, -1.0f,  // top-left
                // front face
                -1.0f, -1.0f, 1.0f, // bottom-left
                1.0f, -1.0f, 1.0f,  // bottom-right
                1.0f, 1.0f, 1.0f,   // top-right
                1.0f, 1.0f, 1.0f,   // top-right
                -1.0f, 1.0f, 1.0f,  // top-left
                -1.0f, -1.0f, 1.0f, // bottom-left
                // left face
                -1.0f, 1.0f, 1.0f,   // top-right
                -1.0f, 1.0f, -1.0f,  // top-left
                -1.0f, -1.0f, -1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, // bottom-left
                -1.0f, -1.0f, 1.0f,  // bottom-right
                -1.0f, 1.0f, 1.0f,   // top-right
                                     // right face
                1.0f, 1.0f, 1.0f,    // top-left
                1.0f, -1.0f, -1.0f,  // bottom-right
                1.0f, 1.0f, -1.0f,   // top-right
                1.0f, -1.0f, -1.0f,  // bottom-right
                1.0f, 1.0f, 1.0f,    // top-left
                1.0f, -1.0f, 1.0f,   // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f, // top-right
                1.0f, -1.0f, -1.0f,  // top-left
                1.0f, -1.0f, 1.0f,   // bottom-left
                1.0f, -1.0f, 1.0f,   // bottom-left
                -1.0f, -1.0f, 1.0f,  // bottom-right
                -1.0f, -1.0f, -1.0f, // top-right
                // top face
                -1.0f, 1.0f, -1.0f, // top-left
                1.0f, 1.0f, 1.0f,   // bottom-right
                1.0f, 1.0f, -1.0f,  // top-right
                1.0f, 1.0f, 1.0f,   // bottom-right
                -1.0f, 1.0f, -1.0f, // top-left
                -1.0f, 1.0f, 1.0f   // bottom-left
            });
            break;
         case SimplePrimitiveType::PLANE:
         case SimplePrimitiveType::PLANE_WITH_ATTRIBUTES:
            vertices = std::vector<float>({
                -1.0f, 0.0f, -1.0f, // top-right
                1.0f, 0.0f, 1.0f,   // bottom-left
                1.0f, 0.0f, -1.0f,  // top-left
                1.0f, 0.0f, 1.0f,   // bottom-left
                -1.0f, 0.0f, -1.0f, // top-right
                -1.0f, 0.0f, 1.0f,  // bottom-right
            });
            break;
         }

         if (SimplePrimitiveType::PLANE_WITH_ATTRIBUTES == typeMesh)
         {
            normals = std::vector<float>({
                0.0f, 0.0f, 1.0f, // top-right
                0.0f, 0.0f, 1.0f, // bottom-left
                0.0f, 0.0f, 1.0f, // top-left
                0.0f, 0.0f, 1.0f, // bottom-left
                0.0f, 0.0f, 1.0f, // top-right
                0.0f, 0.0f, 1.0f, // bottom-right
            });
            texCoords = std::vector<float>({
                1.0f, 0.0f, // top-right
                0.0f, 1.0f, // bottom-left
                0.0f, 0.0f, // top-left
                0.0f, 1.0f, // bottom-left
                1.0f, 0.0f, // top-right
                1.0f, 1.0f, // bottom-right
            });
         }

         BoundingBoxBuilder builder;
         BoundingBox3D boundingBox = builder.Build(vertices);

         const auto &vertexAttributes = arg.mVertexAttributes;
         for (const auto &vertexAttribute : vertexAttributes)
         {
            if (vertexAttribute->GetAttributeType() == eAttributeType::STANDART)
            {
               std::vector<float> data;
               const auto &standartAttribute = std::static_pointer_cast<StandartAttributeDataBase>(vertexAttribute);
					if (standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexPosition)
               {
                  data = std::move(vertices);
               }
               else if (standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexTexCoords)
               {
                  data = std::move(texCoords);
               }
               else if (standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexNormal)
               {
                  data = std::move(normals);
               }

               if (data.size())
               {
                  const auto &vbo = new VertexBufferObject<float>(std::move(data),
                                                                  vertexAttribute->GetAttributeName(),
                                                                  vertexAttribute->GetAttributeIndex(),
                                                                  vertexAttribute->GetAttributeComponentDataType() == eAttributeComponentDataType::FLOAT ? GL_FLOAT : GL_INT,
                                                                  vertexAttribute->GetAttributeComponentsNumber(),
                                                                  GL_ARRAY_BUFFER,
                                                                  eDataCarryFlag::INVALIDATE);
                  vao->AddVBO(vbo);
               }
            }
         }

         assert(vao->GetVertexBufferObjects().size());
         vao->BindBuffersToVao();

         resultSkin = std::make_shared<Skin>(vao, boundingBox, std::to_string(static_cast<int32_t>(arg.mSimplePrimitiveType)));
      }

      return resultSkin;
   }

   void SimplePrimitiveAllocationPolicy::DeallocateMemory(const std::shared_ptr<Skin> &arg)
   {
      LogInfo("SimplePrimitiveAllocationPolicy::DeallocateMemory: ", arg->GetMeshName());
      arg->CleanUp();
   }

}
