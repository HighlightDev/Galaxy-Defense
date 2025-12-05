#include "RuntimeGeneratedMeshAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/OpenGL/AttributesDataDescriptor.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"

#include <gl/glew.h>

#include <vector>

using namespace Graphics::OpenGL;
using namespace Graphics::Mesh;
using namespace EngineCore;

namespace Resources {
std::shared_ptr<Skin> RuntimeGeneratedMeshAllocationPolicy::AllocateMemory(const RuntimeGeneratedMeshPoolParameters& arg)
{
    LogInfo("RuntimeGeneratedMeshAllocationPolicy::AllocateMemory: ", arg.mComponentName);
    std::shared_ptr<Skin> resultSkin;

    {
        const auto vao = std::make_shared<VertexArrayObject>();

        const auto& vertexAttributes = arg.mVertexAttributes;
        for (const auto& vertexAttribute : vertexAttributes) {
            if (vertexAttribute->GetAttributeType() == eAttributeType::STANDART) {
                const auto& standartAttribute = std::static_pointer_cast<StandartAttributeDataBase>(vertexAttribute);
                if (standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexPosition
                    || standartAttribute->GetAttribArrayIndex() == eAttribArrayIndex::VertexTexCoords) {
                    const auto& vbo = new VertexBufferObject<float>(
                        arg.mMaxVerticesCount,
                        vertexAttribute->GetAttributeName(),
                        vertexAttribute->GetAttributeIndex(),
                        vertexAttribute->GetAttributeComponentDataType() == eAttributeComponentDataType::FLOAT ? GL_FLOAT
                                                                                                               : GL_INT,
                        vertexAttribute->GetAttributeComponentsNumber(),
                        GL_ARRAY_BUFFER);
                    vao->AddVBO(vbo);
                }
            }
        }

        if (arg.mMaxIndicesCount > 0) {
            const auto ibo = new IndexBufferObject(std::vector<uint32_t>(arg.mMaxIndicesCount), eDataCarryFlag::INVALIDATE);
            vao->SetIBO(ibo);
        }

        ext_assert(vao->GetVBOs().size(), "Runtime generated mesh VAO must have at least one VBO");
        vao->BindBuffersToVao();

        resultSkin = std::make_shared<Skin>(vao, BoundingBox3D(), arg.mComponentName);
    }

    return resultSkin;
}

void RuntimeGeneratedMeshAllocationPolicy::DeallocateMemory(std::shared_ptr<Skin> arg)
{
    LogInfo("RuntimeGeneratedMeshAllocationPolicy::DeallocateMemory: ", arg->GetMeshName());
    arg->CleanUp();
}

} // namespace Resources
