#include "Skin.h"

namespace Graphics
{
	namespace Mesh
	{

		Skin::Skin(const std::shared_ptr<VertexArrayObject> &vao, const BoundingBox3D &boundingBox, const std::string& meshName)
			: m_buffer(vao),
			  mBoundingBox(boundingBox),
			  mMeshName(meshName)
		{
		}

		Skin::~Skin()
		{
		}
		
		bool Skin::operator==(const Skin& right) const
		{
			return this->m_buffer->GetDescriptor() == right.m_buffer->GetDescriptor();
		}

		BoundingBox3D Skin::GetBoundingBox() const
		{
			return mBoundingBox;
		}

		const std::shared_ptr<VertexArrayObject> &Skin::GetBuffer() const
		{
			return m_buffer;
		}

		std::string Skin::GetMeshName() const
		{
			return mMeshName;
		}

		void Skin::CleanUp()
		{
			m_buffer->CleanUp();
		}
	}
}