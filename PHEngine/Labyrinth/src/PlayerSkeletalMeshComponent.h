#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"

using namespace Game;

namespace Labyrinth
{
   class PlayerSkeletalMeshComponent
      : public SkeletalMeshComponent
   {
      using Base = SkeletalMeshComponent;

   public:

      PlayerSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
         const SkeletalMeshRenderData& renderData);

      virtual ~PlayerSkeletalMeshComponent();

      virtual void Tick(float deltaTime) override;

   };
}