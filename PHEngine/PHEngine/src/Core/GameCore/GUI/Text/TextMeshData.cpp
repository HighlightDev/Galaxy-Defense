#include "TextMeshData.h"

namespace EngineCore
{

	TextMeshData::TextMeshData(const std::vector<float> &vertexPositions, const std::vector<float> &textureCoords)
		: mVertexPositions(vertexPositions),
		  mTextureCoords(textureCoords)
	{
	}

	const std::vector<float> &TextMeshData::GetVertexPositions() const
	{
		return mVertexPositions;
	}

	const std::vector<float> &TextMeshData::GetTextureCoords() const
	{
		return mTextureCoords;
	}

	size_t TextMeshData::GetVertexCount() const
	{
		return mVertexPositions.size() / 2;
	}
}
