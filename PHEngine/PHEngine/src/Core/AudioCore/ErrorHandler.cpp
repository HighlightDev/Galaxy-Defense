#include "ErrorHandler.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore {
void CheckAlErrors()
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        switch (error) {
        case AL_INVALID_NAME:
            LogInfo("OpenAL::CheckAlErrors => AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
            break;
        case AL_INVALID_ENUM:
            LogInfo("OpenAL::CheckAlErrors => AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
            break;
        case AL_INVALID_VALUE:
            LogInfo("OpenAL::CheckAlErrors => AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
            break;
        case AL_INVALID_OPERATION:
            LogInfo("OpenAL::CheckAlErrors => AL_INVALID_OPERATION: the requested operation is not valid");
            break;
        case AL_OUT_OF_MEMORY:
            LogInfo(
                "OpenAL::CheckAlErrors => AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
            break;
        default:
            LogInfo("OpenAL::CheckAlErrors => UNKNOWN AL ERROR: ", error);
        }
    }
}

void CheckAlcErrors(ALCdevice* device)
{
    ALCenum error = alcGetError(device);
    if (error != ALC_NO_ERROR) {
        switch (error) {
        case ALC_INVALID_VALUE:
            LogInfo("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
            break;
        case ALC_INVALID_DEVICE:
            LogInfo("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
            break;
        case ALC_INVALID_CONTEXT:
            LogInfo("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
            break;
        case ALC_INVALID_ENUM:
            LogInfo("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
            break;
        case ALC_OUT_OF_MEMORY:
            LogInfo("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
            break;
        default:
            LogInfo("UNKNOWN ALC ERROR: ", error);
        }

        assert(false);
    }
}
} // namespace EngineCore