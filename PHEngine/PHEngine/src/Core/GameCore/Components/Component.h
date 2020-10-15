#pragma once

#include "ComponentType.h"
#include "Core/GameCore/GameObject.h"
#include "Core/GameCore/ITickable.h"

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

namespace Game
{

	class Actor;
	// This is the base abstract class
	// of all components which could be 
	// picked by actor
	class Component
      : public GameObject
      , public ITickable
	{
		Actor* m_owner;

	public:

      Component(const std::string& gameObjectName);

		virtual ~Component();

      virtual ComponentType GetComponentType() const;

		void SetOwner(Actor* ownerActor);

      void RemoveOwner();

      inline Actor* GetOwner() const;

      Actor* GetBaseOwner() const;

      virtual void PostLevelInit();

	};

   inline Actor* Component::GetOwner() const
   {
      return m_owner;
   }

}

