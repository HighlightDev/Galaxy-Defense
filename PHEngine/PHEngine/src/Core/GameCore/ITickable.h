#pragma once

class ITickable
{
   virtual void Tick(const float deltaTime) = 0;
};
