#pragma once

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"

#include <memory>
#include <utility>

using namespace Graphics::OpenGL;

namespace Graphics {
namespace Mesh {
class TextMesh {
    VertexArrayObject m_buffer;

public:
    TextMesh(const VertexArrayObject& vao);

    virtual ~TextMesh();

    bool operator==(const TextMesh& right) const;

    VertexArrayObject* GetBuffer();

    virtual void CleanUp();
};

} // namespace Mesh
} // namespace Graphics
