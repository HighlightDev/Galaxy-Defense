#pragma once

namespace Graphics
{
	namespace OpenGL
	{
		enum class DataCarryFlag
		{
			//buffer data becomes unavailable after it is sent on a gpu
			Invalidate = 0,
			//buffer data is available
			Store = 1
		};
	}
}