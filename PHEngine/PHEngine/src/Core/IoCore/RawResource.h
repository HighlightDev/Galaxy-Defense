#pragma once

#include "Core/IoCore/AudioLoaderCore/AudioResourceInfo.h"
#include "Core/IoCore/MeshLoaderCore/MeshResourceInfo.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"
#include "ResourceExtensionsInfo.h"

#include <memory>

namespace IO::Audio {
class StbSoundStream;
}

using namespace IO::Audio;

namespace IO {
struct Resource {
    eResourceType mResourceType = eResourceType::UNDEFINED_TYPE;

    void* mData;

    virtual void Clear()
    {
        free(mData);
    }
};

struct TextureResource : public Resource {
    TextureResourceInfo mTexInfo;

    TextureResource()
        : Resource()
    {
        mResourceType = eResourceType::TEXTURE;
    }

    void Clear() override
    {
        free(mData);
    }
};

struct MeshResource : public Resource {
    MeshResource()
        : Resource()
    {
        mResourceType = eResourceType::MESH;
    }

    void Clear() override
    {
        MeshResourceInfo* data = GetMeshResourceInfo();
        delete data;
    }

    MeshResourceInfo* GetMeshResourceInfo() const
    {
        MeshResourceInfo* data = (MeshResourceInfo*)mData;
        return data;
    }
};

struct AudioResource : public Resource {
    AudioResourceInfo AudioInfo;

    AudioResource()
        : Resource()
    {
        mResourceType = eResourceType::AUDIO;
    }

    void Clear() override
    {
        free(mData);
    }
};

struct AudioStreamResource : public AudioResource {
    std::shared_ptr<StbSoundStream> mStream;

    AudioStreamResource()
        : AudioResource()
    {
        mResourceType = eResourceType::AUDIO_STREAM;
    }

    void Clear() override
    {
        free(mData);
    }
};

} // namespace IO