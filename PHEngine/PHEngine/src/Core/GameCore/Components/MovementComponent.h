#pragma once
#include "SceneComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/ScriptingCore/LuaExecutors/LuaScriptExecutor_MovementComponent.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

using namespace EnginePhysics;

namespace Game
{

   class MovementComponent
      : public Component
   {

      std::unordered_map<std::string, std::tuple<Transform, float>> mMovementPoints;

      LuaScriptExecutor_MovementComponent mScriptExecutor;

      std::weak_ptr<SceneComponent> mOwnerRootComponent;
      std::weak_ptr<PhysicsComponent> mOwnerPhysicsComponent;

      std::string mDestinationPoint;
      std::string mLastDestinationPoint;

      glm::vec3 mWorldPosition;
      glm::vec3 mWorldTranslationDelta;
      glm::quat mWorldRotation;

      glm::vec3 mStartPosition;
      glm::quat mStartRotation;

      float mTime;

   public:

      MovementComponent(const std::string& gameObjectName, const std::string& relPathToScript);

      virtual ~MovementComponent();

      virtual uint64_t GetComponentType() const override;

      virtual void Tick(const float deltaTime) override;

      virtual void PostLevelInit() override;

      const std::unordered_map<std::string, std::tuple<Transform, float>>& GetMovementPoints() const;

      void AddMovementPoint(const std::string& pointName, const Transform& t, const float transitionTime);

      void SetDestinationPoint(const std::string& pointName);

      std::string GetDestinationPoint() const;

   private:

      void Move(const float deltaTime);
   };

}
