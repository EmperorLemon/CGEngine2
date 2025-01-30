#include <spdlog/sinks/stdout_color_sinks.h>

#include "logger.hpp"

// logger.cpp
namespace cg::core
{
	std::shared_ptr<spdlog::logger> CGLogger::s_logger = nullptr;

	CGLogger::CGLogger(const char* name)
	{
		m_logger = spdlog::stdout_color_mt(name);
		m_logger->set_level(spdlog::level::trace);
	}

	void CGLogger::Init()
	{
		spdlog::set_pattern("%^[%T] %v%$");

		s_logger = spdlog::stdout_color_mt("ENGINE");
		s_logger->set_level(spdlog::level::trace);
	}

	std::shared_ptr<spdlog::logger>& CGLogger::GetCGLogger()
	{
		return s_logger;
	}
}