#pragma once
#include <stdint.h>
#include <string>
#include <glm\vec2.hpp> 
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat2x2.hpp>
#include <glm\mat3x3.hpp>
#include <glm\mat4x4.hpp>
#include <vector>

namespace Graphics
{
	namespace OpenGL
	{
      
      struct UniformArray
      {
      private:

#if DEBUG
   
         // info for debug 
         std::string m_uniformName;

#endif
         std::vector<int32_t> m_uniformLocations;

      public:

         UniformArray() = default;

         explicit UniformArray(int32_t programDescriptor, size_t uniformsCount, const std::string& uniformName);

         ~UniformArray();

         void LoadUniform(size_t uniformIndex, bool arg);

         void LoadUniform(size_t uniformIndex, float arg);

         void LoadUniform(size_t uniformIndex, int32_t arg);

         void LoadUniform(size_t uniformIndex, const glm::vec2& arg);

         void LoadUniform(size_t uniformIndex, glm::vec2&& arg);

         void LoadUniform(size_t uniformIndex, const glm::vec3& arg);

         void LoadUniform(size_t uniformIndex, glm::vec3&& arg);

         void LoadUniform(size_t uniformIndex, const glm::vec4& arg);

         void LoadUniform(size_t uniformIndex, glm::vec4&& arg);

         void LoadUniform(size_t uniformIndex, const glm::mat2& arg);

         void LoadUniform(size_t uniformIndex, const glm::mat3& arg);

         void LoadUniform(size_t uniformIndex, const glm::mat4& arg);

      };

		struct Uniform
		{
		private:

			int32_t uniformLocation;

		public:

			Uniform();

			explicit Uniform(int32_t programDescriptor, const std::string& uniformName);

			~Uniform();

			void LoadUniform(bool arg);

			void LoadUniform(float arg);

			void LoadUniform(int32_t arg);

         void LoadUniform(size_t arg);

			void LoadUniform(const glm::vec2& arg);

			void LoadUniform(glm::vec2&& arg);

			void LoadUniform(const glm::vec3& arg);

			void LoadUniform(glm::vec3&& arg);

			void LoadUniform(const glm::vec4& arg);

			void LoadUniform(glm::vec4&& arg);

			void LoadUniform(const glm::mat2& arg);

			void LoadUniform(const glm::mat3& arg);

			void LoadUniform(const glm::mat4& arg);

		};
	}
}