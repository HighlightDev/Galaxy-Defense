#include "Skin.h"

namespace Graphics
{
	namespace Mesh
	{

		Skin::Skin(const VertexArrayObject &vao, const BoundingBox &boundingBox)
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

		VertexArrayObject *Skin::GetBuffer()
		{
			return &m_buffer;
		}

		void Skin::CleanUp()
		{
			m_buffer.CleanUp();
		}
	}
}