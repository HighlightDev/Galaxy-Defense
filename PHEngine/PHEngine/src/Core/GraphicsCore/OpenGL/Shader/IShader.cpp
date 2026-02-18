#define _CRT_SECURE_NO_WARNINGS

#include "IShader.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FolderManager.h"

#include <algorithm>
#include <fstream>
#include <set>

using namespace EngineCore;

namespace Graphics {
namespace OpenGL {
IShader::IShader(const std::string& shaderName)
    : m_shaderProgramID(-1)
    , mShaderName(shaderName)
{
}

IShader::~IShader()
{
}

bool IShader::operator==(const IShader& right) const
{
    return this->mShaderName == right.mShaderName && this->m_shaderIdsMap == right.m_shaderIdsMap
        && this->m_shaderProgramID == right.m_shaderProgramID;
}

uint32_t IShader::GetShaderProgramID() const
{
    return m_shaderProgramID;
}

Uniform IShader::GetUniform(const std::string& uniformName, uint32_t shaderProgramID) const
{
    try {
        return Uniform(shaderProgramID, uniformName);
    } catch (std::invalid_argument innerEx) {
        ext_assert(false, "IShader::GetUniform: shaderName = " + mShaderName + " could not bind uniform");
    }
}

UniformArray IShader::GetUniformArray(
    const std::string& uniformName, size_t countOfUniforms, uint32_t shaderProgramID, const eShaderType shaderType) const
{
    static std::unordered_map<eShaderType, GLenum> s_mapShaderTypeToUniformShaderType
        = {{eShaderType::VertexShader, GL_MAX_VERTEX_UNIFORM_COMPONENTS},
           {eShaderType::FragmentShader, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS},
           {eShaderType::GeometryShader, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS},
           {eShaderType::TesselationControlShader, GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS},
           {eShaderType::TesselationEvaluationShader, GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS},
           {eShaderType::ComputeShader, GL_MAX_COMPUTE_UNIFORM_COMPONENTS}};
    ext_assert(s_mapShaderTypeToUniformShaderType.count(shaderType), "IShader::GetUniformArray: Unsupported shader type");
    GLint maxUniforms;
    glGetIntegerv(s_mapShaderTypeToUniformShaderType.at(shaderType), &maxUniforms);
    ext_assert(
        maxUniforms >= static_cast<GLint>(countOfUniforms),
        "Requested amount of uniforms is not supported by this type of shader. maxUniformsSupported: "
            + std::to_string(maxUniforms) + ", requested: " + std::to_string(countOfUniforms)
            + ", shaderType: " + std::to_string(static_cast<int8_t>(shaderType)));
    return UniformArray(shaderProgramID, countOfUniforms, uniformName);
}

int32_t IShader::GetAttributeLocationByName(const std::string& attributeName) const
{
    ext_assert(-1 != m_shaderProgramID, "IShader::GetAttributeLocationByName: Shader program ID is invalid");
    const auto attribLocation = glGetAttribLocation(m_shaderProgramID, attributeName.c_str());
    return attribLocation;
}

std::string IShader::GetShaderName() const
{
    return mShaderName;
}

std::vector<std::string> IShader::LoadShaderSrcVector(const std::string& pathToShader) const
{
    std::ifstream stream(pathToShader);
    std::string line;

    std::vector<std::string> code;

    while (stream.is_open() && getline(stream, line)) {
        code.emplace_back(line);
    }

    stream.clear();
    stream.close();

    return code;
}

bool IShader::ProcessShaderIncludes(std::string& shaderSource)
{
    bool bProcessInclude = false;

    if ("" != shaderSource) {
        std::vector<std::string> sourceVector = EngineUtility::Split(shaderSource, '\n');

        std::string includingSources = "", versionInfo = "";
        std::set<std::string> includes;

        for (auto it = sourceVector.begin(); it != sourceVector.end();) {
            if (EngineUtility::StartsWith(*it, "#version")) {
                versionInfo = *it + "\n";
                it = sourceVector.erase(it);
            } else if (EngineUtility::StartsWith(*it, "#include")) {
                size_t indexName = EngineUtility::IndexOf(*it, " ");
                if (std::string::npos != indexName) {
                    std::string name = it->substr(indexName + 1);

                    if (includes.end() == includes.find(name)) // New include
                    {
                        includes.insert(name);
                        name.erase(std::remove(name.begin(), name.end(), '\"'), name.end()); // remove quotes

                        const std::string& absolutePath = IO::FolderManager::GetInstance()->GetShaderCommonPath() + name;
                        includingSources += LoadShaderSource(absolutePath) + "\n";
                    }
                    it = sourceVector.erase(it);
                }
            } else {
                it++;
            }
        }

        for (auto item : sourceVector) {
            EngineUtility::StringStreamWrapper::ToString(item, "\n");
        }

        shaderSource = versionInfo + includingSources + EngineUtility::StringStreamWrapper::FlushString();
        bProcessInclude = true;
    }

    return bProcessInclude;
}

bool IShader::SendToGpuSingleShaderSource(int32_t shaderId, const std::string& shaderSource) const
{
    bool bLoadResult = false;

    try {
        const char* c_str = shaderSource.c_str();
        glShaderSource(shaderId, 1, &c_str, nullptr);

        bLoadResult = true;
    } catch (...) {
    }

    return bLoadResult;
}

std::string IShader::LoadShaderSource(const std::string& pathToShader) const
{
    std::string result = "";

    if ("" != pathToShader) {
        std::ifstream stream(pathToShader);
        std::string line;

        ext_assert(stream.is_open(), "IShader::LoadShaderSource: Could not open shader file: " + pathToShader);

        while (stream.is_open() && getline(stream, line)) {
            result += line + "\n";
        }

        stream.clear();
        stream.close();
    }

    return result;
}

bool IShader::SendToGpuShadersSources(const std::unordered_map<eShaderType, std::string>& shaderSources) const
{
    bool bAllShadersLoaded = true;
    ext_assert(
        ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"),
        "IShader::SendToGpuShadersSources: Not called from Render thread");

    for (const auto& [shaderType, shaderSource] : shaderSources) {
        const GLenum glShaderType = MapShaderTypeToOpenGLConstant(shaderType);
        m_shaderIdsMap[shaderType] = glCreateShader(glShaderType);
        bAllShadersLoaded &= SendToGpuSingleShaderSource(m_shaderIdsMap.at(shaderType), shaderSource);
    }

    return bAllShadersLoaded;
}

void IShader::WriteShaderSrc(const std::string& pathToShader, const std::string& src) const
{
    std::ofstream writeStream(pathToShader);
    writeStream << src;
}

std::string IShader::InsertPredefinesToSource(
    const std::vector<std::string>& shaderSourceVector,
    const std::vector<ShaderGenericDefineConstant>& constantDefines,
    const std::vector<ShaderGenericDefine>& defines,
    const std::vector<ShaderGenericConstantArray>& constantArrays) const
{
    // src only with macros
    std::vector<ShaderGenericDefineConstant> existingConstantDefines;
    std::vector<ShaderGenericDefine> existingDefines;

    auto shaderSourceVectorCopy = shaderSourceVector;

    for (auto it = shaderSourceVectorCopy.begin(); it != shaderSourceVectorCopy.end();) {
        const auto foundArrayIt = std::find_if(constantArrays.begin(), constantArrays.end(), [=](const auto& array) {
            const auto arrayBeginningStr = "const " + array.m_InnerTypeName + " " + array.m_Name;
            return (it->find(arrayBeginningStr) != std::string::npos);
        });

        if (foundArrayIt != constantArrays.end()) {
            it = shaderSourceVectorCopy.erase(it);
        } else if (EngineUtility::StartsWith(*it, "#define")) {
            size_t indexName = EngineUtility::IndexOf(*it, " ");
            size_t indexValue = EngineUtility::IndexOf(*it, " ", indexName + 1);

            const std::string name = it->substr(indexName + 1, indexValue - indexName - 1);

            if (std::string::npos == indexValue) {
                existingDefines.emplace_back(name, true);
            } else {
                std::string value = it->substr(indexValue + 1);
                existingConstantDefines.emplace_back(name, value);
            }

            // remove all macros from code
            it = shaderSourceVectorCopy.erase(it);
        } else {
            ++it;
        }
    }

    // update values for existing macros
    for (auto new_it = constantDefines.begin(); new_it != constantDefines.end(); ++new_it) {
        for (auto existing_it = existingConstantDefines.begin(); existing_it != existingConstantDefines.end();) {
            if (existing_it->m_Name == new_it->m_Name) {
                existing_it = existingConstantDefines.erase(existing_it);
            } else {
                ++existing_it;
            }
        }

        existingConstantDefines.push_back(*new_it);
    }

    // update defines
    for (auto new_it = defines.begin(); new_it != defines.end(); ++new_it) {
        for (auto existing_it = existingDefines.begin(); existing_it != existingDefines.end();) {
            if (existing_it->m_Name == new_it->m_Name) {
                existing_it = existingDefines.erase(existing_it);
            } else {
                ++existing_it;
            }
        }

        if (new_it->bDefined) {
            existingDefines.push_back(std::move(*new_it));
        }
    }

    for (auto it = existingConstantDefines.begin(); it != existingConstantDefines.end(); ++it) {
        EngineUtility::StringStreamWrapper::ToString("#define ", it->m_Name, " ", it->m_Value, '\n');
    }
    const std::string constantDefinesResult = std::move(EngineUtility::StringStreamWrapper::FlushString());

    for (auto it = existingDefines.begin(); it != existingDefines.end(); ++it) {
        EngineUtility::StringStreamWrapper::ToString("#define ", it->m_Name, '\n');
    }
    const std::string definesResult = std::move(EngineUtility::StringStreamWrapper::FlushString());

    for (const auto& array : constantArrays) {
        EngineUtility::StringStreamWrapper::ToString(array.m_Value, '\n');
    }
    const std::string arraysResult = std::move(EngineUtility::StringStreamWrapper::FlushString());

    std::vector<std::string>::iterator version_it = shaderSourceVectorCopy.begin();

    for (auto it = shaderSourceVectorCopy.begin(); it != shaderSourceVectorCopy.end(); ++it, ++version_it) {
        if (EngineUtility::StartsWith(*it, "#version")) {
            version_it += 2;
            break;
        }
    }

    if (not constantDefinesResult.empty()) {
        shaderSourceVectorCopy.insert(version_it, constantDefinesResult);
    }

    version_it = shaderSourceVectorCopy.begin();
    for (auto it = shaderSourceVectorCopy.begin(); it != shaderSourceVectorCopy.end(); ++it, ++version_it) {
        if (EngineUtility::StartsWith(*it, "#version")) {
            version_it += 2;
            break;
        }
    }

    if (not definesResult.empty()) {
        shaderSourceVectorCopy.insert(version_it, definesResult);
    }

    version_it = shaderSourceVectorCopy.begin();
    for (auto it = shaderSourceVectorCopy.begin(); it != shaderSourceVectorCopy.end(); ++it, ++version_it) {
        if (EngineUtility::StartsWith(*it, "#version")) {
            version_it += 2;
            break;
        }
    }

    if (not arraysResult.empty()) {
        shaderSourceVectorCopy.insert(version_it, arraysResult);
    }

    std::string codeResult = "";

    for (std::vector<std::string>::iterator it = shaderSourceVectorCopy.begin(); it != shaderSourceVectorCopy.end(); ++it) {
        std::string& str = *it;
        str = EngineUtility::TrimEnd(str);

        if (shaderSourceVectorCopy.end() - 1 != it)
            codeResult += str + "\n";
        else
            codeResult += str;
    }

    return codeResult;
}

std::string IShader::InsertCodeSnippetsToSource(
    const std::vector<std::string>& shaderSourceVector, const std::vector<ShaderCodeSnippet>& codeSnippets) const
{
    auto shaderSourceVectorCopy = shaderSourceVector;
    auto it = std::lower_bound(
        shaderSourceVectorCopy.begin(),
        shaderSourceVectorCopy.end(),
        "main()",
        [](const std::string& str, const std::string& lookfor) { return str < lookfor; });
    ext_assert(
        it != shaderSourceVectorCopy.end(),
        "IShader::InsertCodeSnippetsToSource: Could not find main() function in shader source");
    for (const auto& snippet : codeSnippets) {
        shaderSourceVectorCopy.insert(it, snippet.m_CodeLines.begin(), snippet.m_CodeLines.end());
    }
    return EngineUtility::Join(shaderSourceVectorCopy, '\n');
}

void IShader::ModifyShaderSourceWithExtraData(
    std::string& shaderSource,
    const std::vector<ShaderGenericDefineConstant>& constantDefines,
    const std::vector<ShaderGenericDefine>& defines,
    const std::vector<ShaderGenericConstantArray>& constantArrays,
    const std::vector<ShaderCodeSnippet>& codeSnippets) const
{
    shaderSource = InsertPredefinesToSource(EngineUtility::Split(shaderSource, '\n'), constantDefines, defines, constantArrays);
    shaderSource = InsertCodeSnippetsToSource(EngineUtility::Split(shaderSource, '\n'), codeSnippets);
}

void IShader::ModifyShaderFileWithExtraData(
    const std::string& pathToShader,
    const std::vector<ShaderGenericDefineConstant>& constantDefines,
    const std::vector<ShaderGenericDefine>& defines,
    const std::vector<ShaderGenericConstantArray>& constantArrays,
    const std::vector<ShaderCodeSnippet>& codeSnippets) const
{
    if (pathToShader == "")
        return;

    auto shaderSrc = LoadShaderSrcVector(pathToShader);

    const std::string& result = InsertPredefinesToSource(shaderSrc, constantDefines, defines, constantArrays);
    const std::string& finalResult = InsertCodeSnippetsToSource(EngineUtility::Split(result, '\n'), codeSnippets);

    WriteShaderSrc(pathToShader, finalResult);
}

void IShader::CompileShaders()
{
    for (auto& [shaderType, shaderID] : m_shaderIdsMap) {
        glCompileShader(shaderID);
    }
}

void IShader::LinkShaders() const
{
    for (const auto& [shaderType, shaderID] : m_shaderIdsMap) {
        glAttachShader(m_shaderProgramID, shaderID);
    }
    glLinkProgram(m_shaderProgramID);

    for (const auto& [shaderType, shaderID] : m_shaderIdsMap) {
        glDetachShader(m_shaderProgramID, shaderID);
    }
}

std::string IShader::GetCompileLogInfo() const
{
    std::string compileLog;

    EngineUtility::StringStreamWrapper::FlushString(); // Just to clear stream
    bool allShadersCompiled = true;
    for (const auto& [shaderType, shaderID] : m_shaderIdsMap) {
        GLint shader_compiled;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shader_compiled);
        const bool is_compiled = (shader_compiled == GL_TRUE);
        allShadersCompiled &= is_compiled;
        if (not is_compiled) {
            GLint log_length = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<char> message(log_length);
            glGetShaderInfoLog(shaderID, log_length, nullptr, message.data());
            const std::string shaderTypeStr = ShaderTypeToString(shaderType);
            EngineUtility::StringStreamWrapper::ToString(
                "\t" + shaderTypeStr + " : ", std::string(message.begin(), message.end()), "\n\t");
        }
    }

    if (not allShadersCompiled) {
        compileLog += std::string("Unresolved mistakes at : " + mShaderName + '\n')
            + EngineUtility::StringStreamWrapper::FlushString() + "\n";
    }

    return compileLog;
}

std::string IShader::GetLinkLogInfo() const
{
    std::string linkLog;
    EngineUtility::StringStreamWrapper::FlushString(); // Just to clear stream
    GLint isLinked = 0;
    glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(m_shaderProgramID, GL_INFO_LOG_LENGTH, &log_length);

        std::vector<char> message(log_length);
        glGetProgramInfoLog(m_shaderProgramID, log_length, nullptr, message.data());
        EngineUtility::StringStreamWrapper::ToString("Unsolved mistakes at :", mShaderName, "\n", message.data(), "\n");
    }

    linkLog = std::move(EngineUtility::StringStreamWrapper::FlushString());
    linkLog = linkLog != "" ? linkLog + "\n" : linkLog;

    return linkLog;
}

bool IShader::IsShaderCompiled() const
{
    return GetCompileLogInfo().size() <= 0;
}

void IShader::ExecuteShader() const
{
    glUseProgram(m_shaderProgramID);
}

void IShader::StopShader() const
{
    glUseProgram(0);
}

void IShader::AccessAllUniformLocations(uint32_t shaderProgramId)
{
    m_shaderProgramID = shaderProgramId;
#if DEBUG
    const auto compileErrorString = GetCompileLogInfo();
    const auto linkErrorString = GetLinkLogInfo();
    if (compileErrorString != "" || linkErrorString != "") {
        LogInfo("ERROR: Shader compilation errors: ", compileErrorString);
        LogInfo("ERROR: Shader linkage errors: ", linkErrorString);
    }
#endif
}

void IShader::AccessAllSubroutineIndices(uint32_t shaderProgramID)
{
}

void IShader::CleanUp(bool bDeleteShaderProgram)
{
    LogInfo("IShader::CleanUp: shaderName: ", mShaderName, " bDeleteShaderProgram: ", bDeleteShaderProgram);
    StopShader();
    for (const auto& [shaderType, shaderID] : m_shaderIdsMap) {
        glDetachShader(m_shaderProgramID, shaderID);
        glDeleteShader(shaderID);
    }

    if (bDeleteShaderProgram) {
        glDeleteProgram(m_shaderProgramID);
    }
}

} // namespace OpenGL
} // namespace Graphics