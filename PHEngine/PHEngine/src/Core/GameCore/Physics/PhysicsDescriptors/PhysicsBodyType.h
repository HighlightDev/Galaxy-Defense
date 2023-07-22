#pragma once

namespace EnginePhysics
{
   enum class ePhysicsBodyType
   {
      STATIC = 0,    // deactivates when velocity is 0
      DYNAMIC = 1,   // never deactivates
      KINEMATIC = 2, // deactivates + cannot be applied linear velocity to body
      GHOST = 3
   };

   enum class ePhysicsDescriptorType
   {
      UNDEFINED,
      RIGID_BODY_CONTROLLER,
      DYNAMIC_CHARACTER_CONTROLLER,
      GHOST_CONTROLLER
   };
}