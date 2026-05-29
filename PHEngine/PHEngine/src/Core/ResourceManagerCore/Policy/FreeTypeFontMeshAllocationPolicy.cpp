#include "FreeTypeFontMeshAllocationPolicy.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/GUI/FreeTypeText/FreeTypeFont.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/OpenGL/VertexArrayObject.h"
#include "Core/GraphicsCore/OpenGL/VertexBufferObject.h"
#include "Core/GraphicsCore/OpenGL/eAttribArrayIndex.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <gl/glew.h>

#include <vector>

using namespace Graphics::OpenGL;
using namespace EngineCore;
using namespace EngineUtility;

namespace Resources {
std::shared_ptr<FreeTypeFontAtlas> FreeTypeFontMeshAllocationPolicy::AllocateMemory(const FreeTypeFontParams& arg)
{
    LogInfo("FreeTypeFontMeshAllocationPolicy::AllocateMemory: ", arg.FontName, " PixelSize: ", arg.PixelSize);
    VertexArrayObject vao;
    {
        static constexpr size_t verticesPerCharacter = 6;
        const size_t maxFontCharactersCount = EngineConfigHolder::GetInstance()->GetEngineConfig().MaxFontCharactersCount;

        auto* positionsVBO = new VertexBufferObject<float, GL_DYNAMIC_DRAW>(
            maxFontCharactersCount * verticesPerCharacter,
            "VertexPosition",
            (int32_t)eAttribArrayIndex::VertexPosition,
            GL_FLOAT,
            2,
            GL_ARRAY_BUFFER);

        auto* texCoordsVBO = new VertexBufferObject<float, GL_DYNAMIC_DRAW>(
            maxFontCharactersCount * verticesPerCharacter,
            "VertexTexCoords",
            (int32_t)eAttribArrayIndex::VertexTexCoords,
            GL_FLOAT,
            2,
            GL_ARRAY_BUFFER);
        auto* colorVBO = new VertexBufferObject<float, GL_DYNAMIC_DRAW>(
            maxFontCharactersCount * verticesPerCharacter,
            "VertexColor",
            (int32_t)eAttribArrayIndex::VertexColor,
            GL_FLOAT,
            3,
            GL_ARRAY_BUFFER);

        vao.AddVBO(positionsVBO, texCoordsVBO, colorVBO);

        vao.BindBuffersToVao();
    }

    const auto fontFullPathToFile = IO::FolderManager::GetInstance()->GetAbsolutePath(arg.FontName + ".ttf");
    const auto& font = std::make_shared<FreeTypeFont>(fontFullPathToFile);
    return std::make_shared<FreeTypeFontAtlas>(vao, font, arg);
}

void FreeTypeFontMeshAllocationPolicy::DeallocateMemory(const std::shared_ptr<FreeTypeFontAtlas>& arg)
{
    LogInfo(
        "FreeTypeFontMeshAllocationPolicy::DeallocateMemory: font: ",
        arg->GetFontFace()->GetFontFileName(),
        " font size: ",
        arg->GetFontSize());
    arg->CleanUp();
}

} // namespace Resources
