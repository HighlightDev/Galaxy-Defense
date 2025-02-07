#pragma once

#include "Core/CommonCore/Assertion.h"
#include "eAttribArrayIndex.h"

#include <memory>
#include <string>

namespace Graphics::OpenGL {
enum class eAttributeType { STANDART, CUSTOM };

enum class eAttributeComponentDataType { FLOAT, INT };

class AttributeDataBase {
protected:
    int32_t mAttributeIndex;

public:
    explicit AttributeDataBase(const int32_t attributeIndex)
        : mAttributeIndex(attributeIndex)
    {
    }

    int32_t GetAttributeIndex() const
    {
        return mAttributeIndex;
    }

    virtual eAttributeType GetAttributeType() const = 0;

    virtual std::string GetAttributeName() const = 0;

    virtual eAttributeComponentDataType GetAttributeComponentDataType() const = 0;

    virtual int32_t GetAttributeComponentsNumber() const = 0;
};

class CustomAttributeData : public AttributeDataBase {
    std::string mAttributeName;

    eAttributeComponentDataType mAttributeComponentDataType;

    int32_t mAttributeComponentsNumber;

public:
    explicit CustomAttributeData(
        const std::string& attributeName,
        const int32_t attributeIndex,
        const eAttributeComponentDataType type,
        const int32_t attributeComponentsNumber)
        : AttributeDataBase(attributeIndex)
        , mAttributeName(attributeName)
        , mAttributeComponentDataType(type)
        , mAttributeComponentsNumber(attributeComponentsNumber)
    {
        assert(mAttributeComponentsNumber >= 1 && mAttributeComponentsNumber <= 4);
    }

    std::string GetAttributeName() const override
    {
        return mAttributeName;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::CUSTOM;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return mAttributeComponentDataType;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return mAttributeComponentsNumber;
    }
};

class StandartAttributeDataBase : public AttributeDataBase {
public:
    explicit StandartAttributeDataBase(const int32_t attributeIndex)
        : AttributeDataBase(attributeIndex)
    {
    }

    virtual eAttribArrayIndex GetAttribArrayIndex() const = 0;
};

template<eAttribArrayIndex attribName>
class StandartAttributeData;

template<>
class StandartAttributeData<eAttribArrayIndex::VertexPosition> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexPosition";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexPosition;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::FLOAT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 3;
    }
};

template<>
class StandartAttributeData<eAttribArrayIndex::VertexNormal> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexNormal";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexNormal;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::FLOAT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 3;
    }
};

template<>
class StandartAttributeData<eAttribArrayIndex::VertexTexCoords> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexTexCoords";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexTexCoords;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::FLOAT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 2;
    }
};

template<>
class StandartAttributeData<eAttribArrayIndex::VertexTangent> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexTangent";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexTangent;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::FLOAT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 3;
    }
};

template<>
class StandartAttributeData<eAttribArrayIndex::VertexBitangent> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexBitangent";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexBitangent;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::FLOAT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 3;
    }
};

template<>
class StandartAttributeData<eAttribArrayIndex::VertexBlendWeights> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexBlendWeights";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexBlendWeights;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::FLOAT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 4;
    }
};

template<>
class StandartAttributeData<eAttribArrayIndex::VertexBlendIndex> : public StandartAttributeDataBase {
public:
    explicit StandartAttributeData(const int32_t attributeIndex)
        : StandartAttributeDataBase(attributeIndex)
    {
    }

    std::string GetAttributeName() const override
    {
        return "VertexBlendIndex";
    }

    eAttribArrayIndex GetAttribArrayIndex() const override
    {
        return eAttribArrayIndex::VertexBlendIndex;
    }

    eAttributeType GetAttributeType() const override
    {
        return eAttributeType::STANDART;
    }

    eAttributeComponentDataType GetAttributeComponentDataType() const override
    {
        return eAttributeComponentDataType::INT;
    }

    int32_t GetAttributeComponentsNumber() const override
    {
        return 4;
    }
};
} // namespace Graphics::OpenGL
