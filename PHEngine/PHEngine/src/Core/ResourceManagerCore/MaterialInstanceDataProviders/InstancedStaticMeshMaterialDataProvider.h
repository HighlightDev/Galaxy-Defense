#pragma once

#include <atomic>
#include <string>
#include <unordered_map>

#include "MaterialInstanceDataProvider.h"

namespace EngineCore
{
    class EngineObject;
}

namespace Resources
{
    class InstancedStaticMeshMaterialDataProvider : public IMaterialInstanceDataProvider
    {
        std::atomic_bool mIsBeingUpdatedDataOnRenderThread{false};

        std::unordered_map<std::string, std::weak_ptr<::EngineCore::EngineObject>> mStoredPropertiesInEngineObjectsMap;

        int32_t mInstanceId{-1};

        bool mIsInstanceActive{false};

    protected:
        void UpdateData(); // todo: for now

        virtual std::string GetBatchKey() const = 0;

        void BindEngineObjectToPropertyName(const std::shared_ptr<::EngineCore::EngineObject>& engineObject, const std::string& propertyName);

        std::shared_ptr<::EngineCore::EngineObject> GetEngineObjectByPropertyName(const std::string& propertyName) const;

		// this id is identical to draw call instance
		int32_t GetInstanceId() const override;

		bool IsInstanceActive() const override;
    };
}
