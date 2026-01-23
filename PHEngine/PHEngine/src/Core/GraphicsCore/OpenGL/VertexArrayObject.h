#pragma once

#include "Core/CommonCore/Assertion.h"
#include "IndexBufferObject.h"
#include "VertexBufferObject.h"

#include <gl/glew.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace Graphics {
namespace OpenGL {
class VertexArrayObject {
private:
    uint32_t m_descriptor;
    std::vector<std::pair<BufferObjectBase*, std::string>> m_vbos;
    IndexBufferObject* m_ibo;

public:
    VertexArrayObject();
    ~VertexArrayObject();

    uint32_t GetDescriptor() const;

    BufferObjectBase* GetVBOByIndex(const size_t index) const;

    BufferObjectBase* GetVboByAttribArrayIndexName(const std::string& attribArrayIndexName) const
    {
        auto alreadyExistingAttribVboIt = std::find_if(
            m_vbos.begin(), m_vbos.end(), [=](const auto& vboPair) { return vboPair.second == attribArrayIndexName; });

        return alreadyExistingAttribVboIt == m_vbos.end() ? nullptr : alreadyExistingAttribVboIt->first;
    }

    template<typename Arg, typename... Args>
    void AddVBO(Arg&& p_vbo, Args&&... p_vbos)
    {
        if (p_vbo != nullptr) {
            const auto& attribArrayIndexName = static_cast<BufferObjectBase*>(p_vbo)->GetAttribArrayIndexName();
            ext_assert(!GetVboByAttribArrayIndexName(attribArrayIndexName), "VBO already exists for this attribute index");
            m_vbos.emplace_back(std::make_pair(std::forward<Arg>(p_vbo), attribArrayIndexName));
        }
        AddVBO(std::forward<Args>(p_vbos)...);
    }

    template<typename Arg>
    void AddVBO(Arg&& p_vbo)
    {
        if (p_vbo != nullptr) {
            const auto& attribArrayIndexName = static_cast<BufferObjectBase*>(p_vbo)->GetAttribArrayIndexName();
            ext_assert(!GetVboByAttribArrayIndexName(attribArrayIndexName), "VBO already exists for this attribute index");
            m_vbos.emplace_back(std::make_pair(std::forward<Arg>(p_vbo), attribArrayIndexName));
        }
    }

    bool HasIBO() const;

    void SetIBO(IndexBufferObject* const ibo);

    const IndexBufferObject* GetIBO() const;

    const std::vector<std::pair<BufferObjectBase*, std::string>>& GetVBOs() const;

    void GenVAO();

    void RenderVAO(const int32_t primitiveMode = GL_TRIANGLES);

    void RenderVAO(const size_t first, const size_t count, const int32_t primitiveMode = GL_TRIANGLES);

    void RenderInstanced(const int32_t primitiveMode, const size_t primitivesCount);

    void BindBuffersToVao();

    void DisableVertexAttribArrays();

    void CleanUp();
};
} // namespace OpenGL
} // namespace Graphics
