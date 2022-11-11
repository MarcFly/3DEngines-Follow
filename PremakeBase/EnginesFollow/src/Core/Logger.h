#pragma once

#include "../Globals.h"
#include <spdlog/spdlog.h>

namespace Engine {
	struct EF_API Logger {
		static void Init();
		static void Close();

		static std::shared_ptr<spdlog::logger> app_logger;
		static std::shared_ptr<spdlog::logger> engine_logger;
	};
};

#define Engine_TRACE(...)	SPDLOG_LOGGER_TRACE(Engine::Logger::engine_logger, __VA_ARGS__));
#define Engine_ERROR(...)	SPDLOG_LOGGER_ERROR(Engine::Logger::engine_logger, __VA_ARGS__);
#define Engine_WARN(...)	SPDLOG_LOGGER_WARN(Engine::Logger::engine_logger, __VA_ARGS__);
#define Engine_INFO(...)	SPDLOG_LOGGER_INFO(Engine::Logger::engine_logger, __VA_ARGS__);
#define Engine_FATAL(...)	SPDLOG_LOGGER_FATAL(Engine::Logger::engine_logger, __VA_ARGS__);

#define APP_ERROR(...)	SPDLOG_LOGGER_TRACE(Engine::Logger::app_logger, __VA_ARGS__);
#define APP_TRACE(...)	SPDLOG_LOGGER_ERROR(Engine::Logger::app_logger, __VA_ARGS__);
#define APP_WARN(...)	SPDLOG_LOGGER_WARN(Engine::Logger::app_logger, __VA_ARGS__);
#define APP_INFO(...)	SPDLOG_LOGGER_INFO(Engine::Logger::app_logger, __VA_ARGS__);
#define APP_FATAL(...)	SPDLOG_LOGGER_FATAL(Engine::Logger::app_logger, __VA_ARGS__);