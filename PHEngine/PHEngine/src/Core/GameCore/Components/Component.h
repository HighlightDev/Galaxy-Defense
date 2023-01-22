#pragma once

#include "ComponentType.h"
#include "Core/GameCore/EngineObject.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"

#include <memory>

namespace EngineCore
{
   class Scene;
	class Actor;
	// This is the base abstract class
	// of all components which could be 
	// picked by actor
	class Component
      : public EngineObject
      , public ITickable
      , public ISerializable
	{
		std::weak_ptr<Actor> m_owner;

   protected:

      std::shared_ptr<EngineGOProperty<bool>> mIsEnabled;

      bool mIsPostLevelInitialized;

      std::weak_ptr<Scene> m_sceneWP;

	public:

      Component(const std::string& gameObjectName);

		virtual ~Component();

      virtual eComponentType GetComponentType() const;

		virtual void SetOwner(const std::weak_ptr<Actor>& ownerActor);

      void RemoveOwner();

      std::weak_ptr<Actor> GetOwner() const;

      std::weak_ptr<Actor> GetBaseOwner() const;

      bool IsEnabled() const;

      void SetScene(const std::weak_ptr<Scene>& scene);

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override {};

      virtual void SetIsEnabled(const bool bEnabled);

      virtual void OnPostInitialized();

      virtual void PostLevelInit();

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) = 0;

   protected:

      SerializeDataActor& GetSerializeDataActor(SerializeDataContainer& dataContainer);
	};

}

