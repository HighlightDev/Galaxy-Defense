#pragma once

#include <vector>

namespace EngineCore
{

	/**
	 * Stores the vertex data for all the quads on which a text will be rendered.
	 * @author Karl
	 *
	 */
	struct TextMeshData
	{
		std::vector<float> mVertexPositions;
		std::vector<float> mTextureCoords;

		TextMeshData();

		TextMeshData(const std::vector<float> &vertexPositions, const std::vector<float> &textureCoords);
	};
}
