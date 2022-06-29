#include "Skin.h"

namespace Graphics
{
	namespace Mesh
	{

		Skin::Skin(const std::shared_ptr<VertexArrayObject>& vao, const BoundingBox &boundingBox)
			: m_buffer(vao), mBoundingBox(boundingBox)
		{
		}

		Skin::~Skin()
		{
		}

		BoundingBox Skin::GetBoundingBox() const
		{
			return mBoundingBox;
		}

		const std::shared_ptr<VertexArrayObject>& Skin::GetBuffer() const
		{
			return m_buffer;
		}

		void Skin::CleanUp()
		{
			m_buffer->CleanUp();
		}
	}
}