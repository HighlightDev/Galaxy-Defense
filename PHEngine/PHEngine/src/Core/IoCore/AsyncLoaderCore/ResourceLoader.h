#pragma once

#include <string>

namespace IO {

struct Resource;
struct AudioStreamResource;

class ResourceLoader {

public:
    ResourceLoader();

    virtual Resource* LoadResource(const std::string& key) = 0;

    virtual ~ResourceLoader() = default;
};

class TextureResourceLoader : public ResourceLoader {
public:
    TextureResourceLoader();

    Resource* LoadResource(const std::string& key) override;
};

class MeshResourceLoader : public ResourceLoader {
public:
    MeshResourceLoader();

    Resource* LoadResource(const std::string& key) override;
};

class AudioResourceLoader : public ResourceLoader {
public:
    AudioResourceLoader();

    Resource* LoadResource(const std::string& key) override;

    Resource* GetStreamResource(const std::string& key);
};

} // namespace IO
