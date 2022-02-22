#pragma once

#include "ComponentType.h"
#include "Core/GameCore/GameObject.h"
#include "Core/GameCore/ITickable.h"
#include "Core/GameCore/Serialize/ISerializable.h"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

namespace EngineCore
{

	class Actor;
	// This is the base abstract class
	// of all components which could be 
	// picked by actor
	class Component
      : public GameObject
      , public ITickable
      , public ISerializable
	{
		std::weak_ptr<Actor> m_owner;

   protected:

      bool mIsEnabled;

	public:

      Component(const std::string& gameObjectName);

		virtual ~Component();

      virtual ComponentType GetComponentType() const;

		void SetOwner(const std::weak_ptr<Actor>& ownerActor);

      void RemoveOwner();

      std::weak_ptr<Actor> GetOwner() const;

      std::weak_ptr<Actor> GetBaseOwner() const;

      bool IsEnabled() const;

      virtual void SetIsEnabled(const bool bEnabled);

      virtual void OnPostInitialized();

      virtual void PostLevelInit();

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) = 0;

   protected:

      SerializeDataActor& GetSerializeDataActor(SerializeDataContainer& dataContainer);
	};

}

