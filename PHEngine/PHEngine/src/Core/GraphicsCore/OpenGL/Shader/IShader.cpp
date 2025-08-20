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
    : m_vertexShaderID(-1)
    , m_fragmentShaderID(-1)
    , m_geometryShaderID(-1)
    , m_shaderProgramID(-1)
    , mShaderName(shaderName)
{
    LogInfo("IShader::ctor => shaderName = ", shaderName);
}

IShader::~IShader()
{
}

bool IShader::operator==(const IShader& right) const
{
    return this->mShaderName == right.mShaderName && this->m_vertexShaderID == right.m_vertexShaderID
        && this->m_fragmentShaderID == right.m_fragmentShaderID && this->m_shaderProgramID == right.m_shaderProgramID;
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
        LogInfo(
            "IShader::GetUniform => shaderName = ",
            mShaderName,
            " could not bind uniform. Inner exception message : \n",
            innerEx.what());
        throw std::invalid_argument("IShader::GetUniform could not bind uniform");
    }
}

UniformArray IShader::GetUniformArray(
    const std::string& uniformName, size_t countOfUniforms, uint32_t shaderProgramID, const eShaderType shaderType) const
{
    static std::unordered_map<eShaderType, GLenum> s_mapShaderTypeToUniformShaderType
        = {{eShaderType::VertexShader, GL_MAX_VERTEX_UNIFORM_COMPONENTS},
           {eShaderType::FragmentShader, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS},
           {eShaderType::GeometryShader, GL_MAX_GEOMETRY_UNIFORM_COMPONENTS}};
    assert(s_mapShaderTypeToUniformShaderType.count(shaderType));
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
    assert(-1 != m_shaderProgramID);
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
    LogInfo("IShader::SendToGpuSingleShaderSource => shaderId = ", shaderId);
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

        assert(stream.is_open());

        while (stream.is_open() && getline(stream, line)) {
            result += line + "\n";
        }

        stream.clear();
        stream.close();
    }

    return result;
}

bool IShader::SendToGpuShadersSources(std::string& vsSource, std::string& gsSource, std::string& fsSource)
{
    bool bVertexShaderLoaded = true, bFragmentShaderLoaded = true, bGeometryShaderLoaded = true;
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));

    if (vsSource != "") {
        /*Vertex shader load*/
        m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        bVertexShaderLoaded &= SendToGpuSingleShaderSource(m_vertexShaderID, vsSource);
    }

    if (fsSource != "") {
        /*Fragment shader load*/
        m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        bFragmentShaderLoaded &= SendToGpuSingleShaderSource(m_fragmentShaderID, fsSource);
    }

    if (gsSource != "") {
        /*Geometry shader load*/
        m_geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
        bGeometryShaderLoaded &= SendToGpuSingleShaderSource(m_geometryShaderID, gsSource);
    }

    return bVertexShaderLoaded && bFragmentShaderLoaded && bGeometryShaderLoaded;
}

void IShader::WriteShaderSrc(const std::string& pathToShader, const std::string& src) const
{
    std::ofstream writeStream(pathToShader);
    writeStream << src;
}

std::string IShader::GetPredefinedSource(
    std::vector<std::string>& shaderSourceVector,
    const std::vector<ShaderGenericDefineConstant>& constantDefines,
    const std::vector<ShaderGenericDefine>& defines,
    const std::vector<ShaderGenericConstantArray>& constantArrays) const
{
    // src only with macros
    std::vector<ShaderGenericDefineConstant> existingConstantDefines;
    std::vector<ShaderGenericDefine> existingDefines;

    for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end();) {
        const auto foundArrayIt = std::find_if(constantArrays.begin(), constantArrays.end(), [=](const auto& array) {
            const auto arrayBeginningStr = "const " + array.m_InnerTypeName + " " + array.m_Name;
            return (it->find(arrayBeginningStr) != std::string::npos);
        });

        if (foundArrayIt != constantArrays.end()) {
            it = shaderSourceVector.erase(it);
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
            it = shaderSourceVector.erase(it);
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

    std::vector<std::string>::iterator version_it = shaderSourceVector.begin();

    for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it, ++version_it) {
        if (EngineUtility::StartsWith(*it, "#version")) {
            version_it += 2;
            break;
        }
    }

    if ("" != constantDefinesResult) {
        shaderSourceVector.insert(version_it, constantDefinesResult);
    }

    version_it = shaderSourceVector.begin();
    for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it, ++version_it) {
        if (EngineUtility::StartsWith(*it, "#version")) {
            version_it += 2;
            break;
        }
    }

    if ("" != definesResult) {
        shaderSourceVector.insert(version_it, definesResult);
    }

    version_it = shaderSourceVector.begin();
    for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it, ++version_it) {
        if (EngineUtility::StartsWith(*it, "#version")) {
            version_it += 2;
            break;
        }
    }

    if ("" != arraysResult) {
        shaderSourceVector.insert(version_it, arraysResult);
    }

    std::string codeResult = "";

    for (std::vector<std::string>::iterator it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it) {
        std::string& str = *it;
        str = EngineUtility::TrimEnd(str);

        if (shaderSourceVector.end() - 1 != it)
            codeResult += str + "\n";
        else
            codeResult += str;
    }

    return codeResult;
}

void IShader::ProcessPredefineToSource(
    std::string& shaderSource,
    const std::vector<ShaderGenericDefineConstant>& constantDefines,
    const std::vector<ShaderGenericDefine>& defines,
    const std::vector<ShaderGenericConstantArray>& constantArrays) const
{
    auto shaderSrc = EngineUtility::Split(shaderSource, '\n');

    shaderSource = GetPredefinedSource(shaderSrc, constantDefines, defines, constantArrays);
}

void IShader::ProcessPredefineToFile(
    const std::string& pathToShader,
    const std::vector<ShaderGenericDefineConstant>& constantDefines,
    const std::vector<ShaderGenericDefine>& defines,
    const std::vector<ShaderGenericConstantArray>& constantArrays) const
{
    if (pathToShader == "")
        return;

    auto shaderSrc = LoadShaderSrcVector(pathToShader);

    const std::string& result = GetPredefinedSource(shaderSrc, constantDefines, defines, constantArrays);

    WriteShaderSrc(pathToShader, result);
}

void IShader::CompileShaders()
{
    glCompileShader(m_vertexShaderID);
    glCompileShader(m_fragmentShaderID);
    glCompileShader(m_geometryShaderID);
}

void IShader::LinkShaders() const
{
    glAttachShader(m_shaderProgramID, m_vertexShaderID);
    glAttachShader(m_shaderProgramID, m_fragmentShaderID);
    if (m_geometryShaderID != -1) {
        glAttachShader(m_shaderProgramID, m_geometryShaderID);
    }
    glLinkProgram(m_shaderProgramID);

    glDetachShader(m_shaderProgramID, m_vertexShaderID);
    glDetachShader(m_shaderProgramID, m_fragmentShaderID);
    if (m_geometryShaderID != -1) {
        glDetachShader(m_shaderProgramID, m_geometryShaderID);
    }
}

std::string IShader::GetCompileLogInfo() const
{
    std::string compileLog;

    EngineUtility::StringStreamWrapper::FlushString(); // Just to clear stream
    GLint vertex_compiled = 0;
    if (m_vertexShaderID != -1) {
        glGetShaderiv(m_vertexShaderID, GL_COMPILE_STATUS, &vertex_compiled);
        if (vertex_compiled != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(m_vertexShaderID, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> message(log_length);
            glGetShaderInfoLog(m_vertexShaderID, log_length, nullptr, message.data());
            EngineUtility::StringStreamWrapper::ToString(
                "\tVertex shader : ", std::string(message.begin(), message.end()), "\n\t");
        }
    }

    /*Fragment shader log info*/
    GLint fragment_compiled = 0;
    if (m_fragmentShaderID != -1) {
        glGetShaderiv(m_fragmentShaderID, GL_COMPILE_STATUS, &fragment_compiled);
        if (fragment_compiled != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(m_fragmentShaderID, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> message(log_length);
            glGetShaderInfoLog(m_fragmentShaderID, log_length, nullptr, message.data());
            EngineUtility::StringStreamWrapper::ToString(
                "\tFragment shader : ", std::string(message.begin(), message.end()), "\n\t");
        }
    }

    GLint geometry_compiled = 0;
    /*Geometry shader log info*/
    if (m_geometryShaderID != -1) {
        glGetShaderiv(m_geometryShaderID, GL_COMPILE_STATUS, &geometry_compiled);
        if (geometry_compiled != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(m_geometryShaderID, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> message(log_length);
            glGetShaderInfoLog(m_geometryShaderID, log_length, nullptr, message.data());
            EngineUtility::StringStreamWrapper::ToString(
                "\tGeometry shader : ", std::string(message.begin(), message.end()), "\n\t");
        }
    }

    if ((m_vertexShaderID != -1 && vertex_compiled != GL_TRUE) || (m_fragmentShaderID != -1 && fragment_compiled != GL_TRUE)
        || (m_geometryShaderID != -1 && geometry_compiled != GL_TRUE)) {
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
    LogInfo("IShader::AccessAllUniformLocations => shaderProgramId = ", shaderProgramId);
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
    LogInfo("IShader::CleanUp => shaderProgramID = ", m_shaderProgramID, " bDeleteShaderProgram = ", bDeleteShaderProgram);
    StopShader();
    glDetachShader(m_shaderProgramID, m_vertexShaderID);
    glDetachShader(m_shaderProgramID, m_fragmentShaderID);
    if (m_geometryShaderID != -1) {
        glDetachShader(m_shaderProgramID, m_geometryShaderID);
        glDeleteShader(m_geometryShaderID);
    }

    glDeleteShader(m_vertexShaderID);
    glDeleteShader(m_fragmentShaderID);

    if (bDeleteShaderProgram) {
        glDeleteShader(m_shaderProgramID);
    }
}

} // namespace OpenGL
} // namespace Graphics