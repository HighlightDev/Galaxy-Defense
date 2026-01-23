#include "ScreenQuad.h"

#include "Core/GameCore/BoundingBoxBuilder.h"
#include "Core/GraphicsCore/OpenGL/BufferObjectBase.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/eAttribArrayIndex.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/ShaderPool.h"

#include <memory>
#include <utility>
#include <vector>

using namespace Common;
using namespace Resources;
using namespace EngineCore;
using namespace Graphics::OpenGL;

namespace Graphics {
ScreenQuad* ScreenQuad::m_instance = nullptr;

ScreenQuad::ScreenQuad()
    : m_vao(new VertexArrayObject())
{
    Init();
}

ScreenQuad::~ScreenQuad()
{
    delete m_vao;
}

VertexArrayObject* ScreenQuad::GetBuffer() const
{
    return m_vao;
}

void ScreenQuad::Init()
{
    const auto folderManager = IO::FolderManager::GetInstance();

    /*Screen fill quad*/
    std::vector<float> vertices
        = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f};

    std::vector<float> texCoords = {0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1};

    VertexBufferObject<float>* verticesVBO = new VertexBufferObject<float>(
        std::move(vertices),
        "VertexPosition",
        (int32_t)eAttribArrayIndex::VertexPosition,
        GL_FLOAT,
        3,
        GL_ARRAY_BUFFER,
        eDataCarryFlag::INVALIDATE);

    BufferObjectBase* texCoordsVBO = new VertexBufferObject<float>(
        std::move(texCoords),
        "VertexTexCoords",
        (int32_t)eAttribArrayIndex::VertexTexCoords,
        GL_FLOAT,
        2,
        GL_ARRAY_BUFFER,
        eDataCarryFlag::INVALIDATE);

    m_vao->AddVBO(verticesVBO, texCoordsVBO);

    m_vao->BindBuffersToVao();
}
} // namespace Graphics
