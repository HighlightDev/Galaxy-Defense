#include "Skin.h"

namespace Graphics
{
	namespace Mesh
	{

		Skin::Skin(std::unique_ptr<VertexArrayObject>&& vao)
			: m_buffer(std::move(vao))
		{
		}

		Skin::~Skin()
		{
		}

		void Skin::CleanUp()
		{
			m_buffer->CleanUp();
		}
	}
}