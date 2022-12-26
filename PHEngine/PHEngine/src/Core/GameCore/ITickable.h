#pragma once

class ITickable
{
public:

   virtual void Tick(const float deltaTime) = 0;

   virtual void UnpausableTick(const float deltaTime) = 0;
};
