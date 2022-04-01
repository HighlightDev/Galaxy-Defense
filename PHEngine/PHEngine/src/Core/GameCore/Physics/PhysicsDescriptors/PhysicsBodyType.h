#pragma once 
 
 namespace EnginePhysics
{
 enum class ePhysicsBodyType
   {
      STATIC,  // deactivates when velocity is 0
      DYNAMIC, // never deactivates
      KINEMATIC, // deactivates + cannot be applied linear velocity to body
      GHOST
   };
}