#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Io
{
	namespace MeshLoader {

		namespace Assimp {

			struct AnimationFrameLOADER
			{
				double TimeStart;
				glm::vec3 Translation;
				glm::quat Rotation;
				glm::vec3 Scale;

				AnimationFrameLOADER(double timeStart, glm::vec3&& translation, glm::quat&& rotation, glm::vec3&& scale);

				~AnimationFrameLOADER();
			};
		}
	}
}
