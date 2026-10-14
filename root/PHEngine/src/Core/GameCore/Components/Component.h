#pragma once

#include "ComponentType.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"

#include <memory>

namespace EngineCore {
class Scene;
class Actor;
namespace Scripts {
class LuaProxy;
} // namespace Scripts
// This is the base abstract class
// of all components which could be
// picked by actor
class Component : public EngineObject, public Scripts::EngineToLuaReplicatorBase, public ITickable {
    std::weak_ptr<Actor> m_owner;

protected:
    std::shared_ptr<EngineObjectProperty<bool>> mIsEnabled;

    std::weak_ptr<Scene> m_sceneWP;

public:
    explicit Component(const std::string& gameObjectName, const bool isEnabled = true);

    virtual ~Component();

    virtual void OnRegistered();

    virtual void OnUnregistered();

    void CleanUp() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override
    {
        // No properties to sync in base class
    }

    std::shared_ptr<Scripts::LuaProxy> ReplicateLuaProxy() override
    {
        return nullptr;
    }

    virtual eComponentType GetComponentType() const;

    virtual void SetOwner(const std::weak_ptr<Actor>& ownerActor);

    virtual void OnSceneOwnerInitialized();

    void RemoveOwner();

    std::weak_ptr<Actor> GetOwner() const;

    std::weak_ptr<Actor> GetBaseOwner() const;

    bool IsEnabled() const;

    void SetScene(const std::weak_ptr<Scene>& scene);

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    void UnpausableTick(const float deltaTimeSec, const float playSpeed) override{};

    virtual void SetIsEnabled(const bool bEnabled);

    virtual void OnPostRegistered();

    virtual void OnPostOwnerInitialized();

    virtual void PostLevelInit();
};

} // namespace EngineCore
