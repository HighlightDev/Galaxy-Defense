#pragma once

#include <memory>
#include <utility>

#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"

using namespace Graphics::OpenGL;

namespace Graphics
{
    namespace Mesh
    {
        class TextMesh
        {
            VertexArrayObject m_buffer;

        public:
            TextMesh(const VertexArrayObject &vao);

            virtual ~TextMesh();

            VertexArrayObject *GetBuffer();

            virtual void CleanUp();
        };

    }
}
