#pragma once

class ITickable
{
public:

   virtual void Tick(const float deltaTime) = 0;
};
