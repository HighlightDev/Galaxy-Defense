#pragma once

#include <stdint.h>
#include <cstdint>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <tuple>

namespace EngineCore
{
    class StaticMeshComponent;
    class Scene;
    class Actor;
}

namespace Game
{
    class TowersHandler
    {
        std::weak_ptr<::EngineCore::Scene> mSceneWp;

        std::shared_ptr<::EngineCore::Actor> mTowersActor;

        int32_t mTowerComponentCounter{0};

        std::string mNewTowerName;

        glm::vec3 mNewTowerColor;

        std::stack<std::tuple<std::string, std::shared_ptr<::EngineCore::StaticMeshComponent>>> mActiveTowersComponents;

        std::stack<std::tuple<std::string, std::shared_ptr<::EngineCore::StaticMeshComponent>>> mIdleTowerComponents;

    public:
        explicit TowersHandler(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                               const std::shared_ptr<::EngineCore::Actor> &towersActor);

        void CreateNewTower(const glm::vec3 &position,
                            const glm::vec3 &scale);

        void UndoLastTowerComponent();

        std::unordered_map<std::string, glm::vec3> CollectTowerPoints() const;

        void SetNewTowerColor(const glm::vec3 &color);

        void SetNewTowerName(const std::string &newTowerName);
    };
}
