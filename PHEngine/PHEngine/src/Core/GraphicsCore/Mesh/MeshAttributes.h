#pragma once

#include <stdint.h>

#include <cstddef>
#include <vector>

namespace MeshLoader {
namespace Assimp {
struct MeshDataCollector;
}
} // namespace MeshLoader

namespace Graphics {
namespace Mesh {

using MeshLoader::Assimp::MeshDataCollector;

struct MeshAttributes {
    std::vector<float> BoneWeights;

    std::vector<int32_t> BoneIndices;

    std::vector<uint32_t> VertexIndices;

    std::vector<float> Positions;

    std::vector<float> TextureCoordinates;

    std::vector<float> Normals;

    std::vector<float> TangentNormals;

    std::vector<float> BitangetNormals;

    MeshAttributes(const struct MeshDataCollector& collector);
};

} // namespace Mesh
} // namespace Graphics
