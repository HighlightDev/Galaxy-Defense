#pragma once

#include <string>
#include <sstream>
#include <xutility>

namespace EngineUtility
{

	struct StringStreamWrapper
	{
		static std::stringstream m_stringStream;

		template <typename Arg, typename ...Args>
		static void ToString(Arg&& arg, Args... args)
		{
			m_stringStream << arg;
			ToString(std::forward<Args>(args)...);
		}

		template <typename Arg>
		static void ToString(Arg&& arg)
		{
			m_stringStream << arg;
		}

		static std::string FlushString()
		{
			std::string result = std::move(m_stringStream.str());
			m_stringStream = std::stringstream();
			return result;
		}
	};

}
