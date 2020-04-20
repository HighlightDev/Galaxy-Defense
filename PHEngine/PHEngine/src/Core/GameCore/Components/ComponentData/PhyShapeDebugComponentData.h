#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhyShapeBase.h"

#include <string>
#include <glm/vec3.hpp>
#include <gl/glew.h>

namespace Game
{
   struct PhyShapeDebugComponentData : public ComponentData
   {
      PhyShapeDebugComponentData(PhyShapeBase* shape)
        : ComponentData()
        , mShape(shape)
      {
      }

      virtual uint64_t GetType() const override {

         return PRIMITIVE_COMPONENT;
      }

      PhyShapeBase* mShape;
   };

}