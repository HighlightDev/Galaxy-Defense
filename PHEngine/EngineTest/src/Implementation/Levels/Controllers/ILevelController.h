#pragma once

namespace TestFeatures {
class ILevelController {
public:
    virtual void OnPreLevelInit() = 0;

    virtual void OnLevelInit() = 0;

    virtual void OnPostLevelInit() = 0;

    virtual void PostPlayLevelFinished() = 0;

    virtual void CleanUp() = 0;
};
} // namespace TestFeatures