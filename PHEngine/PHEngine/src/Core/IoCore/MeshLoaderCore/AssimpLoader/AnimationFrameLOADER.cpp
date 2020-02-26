#include "AnimationFrameLOADER.h"

namespace Io
{
	namespace MeshLoader
	{
		namespace Assimp {

			AnimationFrameLOADER::AnimationFrameLOADER(double timeStart, glm::vec3&& translation, glm::quat&& rotation, glm::vec3&& scale)
				: TimeStart(std::move(timeStart))
				, Translation(std::move(translation))
				, Rotation(std::move(rotation))
				, Scale(std::move(scale))
			{
			}

			AnimationFrameLOADER::~AnimationFrameLOADER()
			{
			}

		}
	}
}
