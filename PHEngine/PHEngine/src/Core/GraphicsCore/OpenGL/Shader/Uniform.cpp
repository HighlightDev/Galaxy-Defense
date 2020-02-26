#include "Uniform.h"
#include "Core/UtilityCore/StringStreamWrapper.h"

#include <gl/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace EngineUtility;

namespace Graphics
{
	namespace OpenGL
	{
      /* UNIFORM ARRAY */

      UniformArray::UniformArray(int32_t programDescriptor, size_t uniformsCount, const std::string& uniformName)
      {
         std::string uniformNamePart = uniformName;

#if DEBUG
         m_uniformName = uniformName;
#endif

         for (size_t i = 0; i < uniformsCount; i++)
         {
            StringStreamWrapper::ToString(uniformNamePart, "[", i, "]");
            std::string readyUniformPart = StringStreamWrapper::FlushString();
            m_uniformLocations.emplace_back(glGetUniformLocation(programDescriptor, readyUniformPart.c_str()));
         }
      }

      UniformArray::~UniformArray()
      {
      }

      void UniformArray::LoadUniform(size_t uniformIndex, bool arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform1i(uniformLocation, arg);
      }

      void UniformArray::LoadUniform(size_t uniformIndex, float arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform1f(uniformLocation, arg);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, int32_t arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform1i(uniformLocation, arg);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, const glm::vec2& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform2f(uniformLocation, arg.x, arg.y);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, glm::vec2&& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform2f(uniformLocation, arg.x, arg.y);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, const glm::vec3& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform3f(uniformLocation, arg.x, arg.y, arg.z);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, glm::vec3&& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform3f(uniformLocation, arg.x, arg.y, arg.z);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, const glm::vec4& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform4f(uniformLocation, arg.x, arg.y, arg.z, arg.w);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, glm::vec4&& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniform4f(uniformLocation, arg.x, arg.y, arg.z, arg.w);
      }
      void UniformArray::LoadUniform(size_t uniformIndex, const glm::mat2& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniformMatrix2fv(uniformLocation, 1, false, glm::value_ptr(arg));
      }
      void UniformArray::LoadUniform(size_t uniformIndex, const glm::mat3& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniformMatrix3fv(uniformLocation, 1, false, glm::value_ptr(arg));
      }

      void UniformArray::LoadUniform(size_t uniformIndex, const glm::mat4& arg)
      {
#if DEBUG
         if (uniformIndex >= m_uniformLocations.size())
         {
            std::cout << "Out of uniform array bounds. Uniform name is : " << m_uniformName << std::endl;
            return;
         }
#endif

         int32_t uniformLocation = m_uniformLocations[uniformIndex];

         if (uniformLocation == -1)
            return;
         glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(arg));
      }

      /* UNIFORM */

		Uniform::Uniform() {}

		Uniform::Uniform(int32_t programDescriptor, const std::string& uniformName)
		{
			uniformLocation = glGetUniformLocation(programDescriptor, uniformName.c_str());
		/*	if (uniformLocation < 0)
				throw new std::invalid_argument(std::string("Could not bind uniform " + uniformName));*/
		}

		Uniform::~Uniform()
		{
		}

		void Uniform::LoadUniform(bool arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform1i(uniformLocation, arg);
		}
		void Uniform::LoadUniform(float arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform1f(uniformLocation, arg);
		}
		void Uniform::LoadUniform(int32_t arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform1i(uniformLocation, arg);
		}
      void Uniform::LoadUniform(size_t arg)
      {
         if (uniformLocation == -1)
            return;
         glUniform1ui(uniformLocation, arg);
      }
		void Uniform::LoadUniform(const glm::vec2& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform2f(uniformLocation, arg.x, arg.y);
		}
		void Uniform::LoadUniform(glm::vec2&& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform2f(uniformLocation, arg.x, arg.y);
		}
		void Uniform::LoadUniform(const glm::vec3& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform3f(uniformLocation, arg.x, arg.y, arg.z);
		}
		void Uniform::LoadUniform(glm::vec3&& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform3f(uniformLocation, arg.x, arg.y, arg.z);
		}
		void Uniform::LoadUniform(const glm::vec4& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform4f(uniformLocation, arg.x, arg.y, arg.z, arg.w);
		}
		void Uniform::LoadUniform(glm::vec4&& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniform4f(uniformLocation, arg.x, arg.y, arg.z, arg.w);
		}
		void Uniform::LoadUniform(const glm::mat2& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniformMatrix2fv(uniformLocation, 1, false, glm::value_ptr(arg));
		}
		void Uniform::LoadUniform(const glm::mat3& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniformMatrix3fv(uniformLocation, 1, false, glm::value_ptr(arg));
		}
		void Uniform::LoadUniform(const glm::mat4& arg)
		{
			if (uniformLocation == -1)
				return;
			glUniformMatrix4fv(uniformLocation, 1, false, glm::value_ptr(arg));
		}
	}
}