#pragma once

/**
 * @brief Interface for objects that require periodic updates (ticks).
 *
 * Classes implementing ITickable must define behavior for both regular and unpausable ticks.
 */
class ITickable {
public:
    /**
     * @brief Called every frame to update the object.
     * @param deltaTimeSec Time elapsed since the last tick, in seconds.
     */
    virtual void Tick(const float deltaTimeSec) = 0;

    /**
     * @brief Called every frame to update the object, even when the game is paused.
     * @param deltaTimeSec Time elapsed since the last tick, in seconds.
     */
    virtual void UnpausableTick(const float deltaTimeSec) = 0;
};
