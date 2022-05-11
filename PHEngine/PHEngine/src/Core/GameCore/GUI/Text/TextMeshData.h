#pragma once

#include <vector>

namespace EngineCore
{

	/**
	 * Stores the vertex data for all the quads on which a text will be rendered.
	 * @author Karl
	 *
	 */
	class TextMeshData
	{
		std::vector<float> mVertexPositions;
		std::vector<float> mTextureCoords;

	public:
		TextMeshData(const std::vector<float> &vertexPositions, const std::vector<float> &textureCoords);

		const std::vector<float> &GetVertexPositions() const;

		const std::vector<float> &GetTextureCoords() const;

		size_t GetVertexCount() const;
	};
}
