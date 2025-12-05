#pragma once

#include "ComponentType.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/ITickable.h"

#include <memory>

namespace EngineCore {
class Scene;
class Actor;
// This is the base abstract class
// of all components which could be
// picked by actor
class Component : public EngineObject, public ITickable, public std::enable_shared_from_this<Component> {
    std::weak_ptr<Actor> m_owner;

protected:
    std::shared_ptr<EngineObjectProperty<bool>> mIsEnabled;

    std::weak_ptr<Scene> m_sceneWP;

public:
    Component(const std::string& gameObjectName);

    virtual ~Component();

    virtual void OnRegistered();

    virtual void OnUnregistered();

    virtual void CleanUp();

    virtual eComponentType GetComponentType() const;

    virtual void SetOwner(const std::weak_ptr<Actor>& ownerActor);

    virtual void OnSceneOwnerInitialized();

    void RemoveOwner();

    std::weak_ptr<Actor> GetOwner() const;

    std::weak_ptr<Actor> GetBaseOwner() const;

    bool IsEnabled() const;

    void SetScene(const std::weak_ptr<Scene>& scene);

    void Tick(const float deltaTimeSec) override;

    void UnpausableTick(const float deltaTimeSec) override{};

    virtual void SetIsEnabled(const bool bEnabled);

    virtual void OnPostRegistered();

    virtual void OnPostOwnerInitialized();

    virtual void PostLevelInit();
};

} // namespace EngineCore
