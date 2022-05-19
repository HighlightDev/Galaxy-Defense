#include "TextMeshData.h"

namespace EngineCore
{
	TextMeshData::TextMeshData()
		: mVertexPositions(),
		  mTextureCoords()
	{
	}

	TextMeshData::TextMeshData(const std::vector<float> &vertexPositions, const std::vector<float> &textureCoords)
		: mVertexPositions(vertexPositions),
		  mTextureCoords(textureCoords)
	{
	}
}
