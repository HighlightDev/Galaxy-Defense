#include "TextMeshData.h"

namespace EngineCore
{
	TextMeshData::TextMeshData()
		: mVertexPositions(),
		  mTextureCoords()
	{
	}

	TextMeshData::TextMeshData(const std::vector<float> &vertexPositions,
							   const std::vector<float> &textureCoords,
							   const float textWidth,
							   const float textHeight)
		: mVertexPositions(vertexPositions),
		  mTextureCoords(textureCoords),
		  mTextWidth(textWidth),
		  mTextHeight(textHeight)
	{
	}
}
