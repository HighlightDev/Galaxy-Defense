#include "Skin.h"

namespace Graphics
{
	namespace Mesh
	{

		Skin::Skin(const VertexArrayObject& vao)
			: m_buffer(vao)
		{
		}

		Skin::~Skin()
		{
		}

		void Skin::CleanUp()
		{
			m_buffer.CleanUp();
		}
	}
}