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

    virtual ~Resource()
    {
        free(mData);
        mData = nullptr;
    }
};

struct TextureResource : public Resource {
    TextureResourceInfo mTexInfo;

    TextureResource()
        : Resource()
    {
        mResourceType = eResourceType::TEXTURE;
    }

    ~TextureResource() override
    {
        free(mData);
        mData = nullptr;
    }
};

struct MeshResource : public Resource {
    MeshResource()
        : Resource()
    {
        mResourceType = eResourceType::MESH;
    }

    ~MeshResource() override
    {
        MeshResourceInfo* data = GetMeshResourceInfo();
        delete data;
        mData = nullptr;
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

    ~AudioResource() override
    {
        free(mData);
        mData = nullptr;
    }
};

struct AudioStreamResource : public AudioResource {
    std::shared_ptr<StbSoundStream> mStream;

    AudioStreamResource()
        : AudioResource()
    {
        mResourceType = eResourceType::AUDIO_STREAM;
    }

    ~AudioStreamResource() override
    {
        mStream.reset();
        free(mData);
        mData = nullptr;
    }
};

} // namespace IO