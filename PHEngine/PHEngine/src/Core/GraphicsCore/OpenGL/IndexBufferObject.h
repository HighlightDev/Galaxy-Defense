#pragma once
#include "BufferObjectBase.h"
#include "DataCarryFlag.h"

#include <stdint.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace Graphics {
namespace OpenGL {
class IndexBufferObject : public BufferObjectBase {
private:
    std::vector<uint32_t> m_data;
    eDataCarryFlag m_dataCarryFlag;

    size_t m_countOfIndices;
    size_t m_countOfTotalLengthOfData;

public:
    IndexBufferObject(const std::vector<uint32_t>& indicesData, eDataCarryFlag dataCarryFlag = eDataCarryFlag::INVALIDATE);

    ~IndexBufferObject() override;

    void GenIndexBuffer();

    void BindIndexBuffer();

    void SendDataToGPU() override;

    static void UnbindIndexBuffer();

    void CleanUp() override;

    size_t GetCountOfIndices() const override;

    size_t GetElementByteSize() const override;

    size_t GetTotalLengthOfData() const override;

    size_t GetVectorSize() const override;

    size_t GetVertexAttribIndex() const override;
};
} // namespace OpenGL
} // namespace Graphics
