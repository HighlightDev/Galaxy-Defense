#pragma once

#include "Core/GraphicsCore/OpenGL/Shader/VertexFactoryMaterialCompositeShader.h"
#include "Core/GameCore/ShaderImplementation/SimpleShader.h"
#include "Core/GameCore/ShaderImplementation/VertexFactoryImp/StaticMeshVertexFactory.h"
#include "Core/GraphicsCore/RenderData/MeshRenderData.h"

using namespace Graphics::Data;
using namespace EngineCore::ShaderImpl;

namespace Graphics::GeometryBatching
{
    class InstancedGeometryBatch
    {
        using ShaderType = VertexFactoryMaterialCompositeShader<StaticMeshVertexFactory, SimpleShader>;

        MeshRenderData m_renderData;

    public:
    private:
        std::shared_ptr<ShaderType> GetShader() const;
    };
}
