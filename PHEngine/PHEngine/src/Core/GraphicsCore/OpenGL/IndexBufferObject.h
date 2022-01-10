#pragma once
#include "VertexBufferObjectBase.h"
#include "DataCarryFlag.h"

#include <memory>
#include <vector>
#include <stdint.h>
#include <cstddef>

namespace Graphics
{
	namespace OpenGL
	{
		class IndexBufferObject :
			public VertexBufferObjectBase
		{
		private:

			std::vector<uint32_t> m_data;
			DataCarryFlag m_dataCarryFlag;

			size_t m_countOfIndices;
			size_t m_countOfTotalLengthOfData;

		public:

			IndexBufferObject(const std::vector<uint32_t>& indicesData, DataCarryFlag dataCarryFlag = DataCarryFlag::Invalidate);

			virtual ~IndexBufferObject();

			void GenIndexBuffer();

			void BindIndexBuffer();

			void SendDataToGPU() override;

			static void UnbindIndexBuffer();

			void CleanUp() override;

			virtual size_t GetCountOfIndices() const override;

			virtual size_t GetTotalLengthOfData() const override;

			virtual size_t GetVectorSize() const override;

			virtual size_t GetVertexAttribIndex() const override;
		};
	}
}

