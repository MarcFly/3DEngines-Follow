#include <EnginePCH.h>
#include "Logger.h"
#include <spdlog/spdlog.h>

using namespace Engine;

std::shared_ptr<spdlog::logger> Logger::engine_logger;
std::shared_ptr<spdlog::logger> Logger::app_logger;


#include <spdlog/sinks/stdout_color_sinks.h>

void Logger::Init() {
	spdlog::set_pattern("%^[%T] %n // [%s:%#]: %v%$"); //[%!] for function name...
	// STARTCOLORX [Time] loggername: text STOPCOLORX

	engine_logger = spdlog::stdout_color_mt("Engine");
	
	app_logger = spdlog::stdout_color_mt("App");

	engine_logger->warn("Initialized Engine Logger");

	app_logger->info("Initialized Engine Logger");
}


void Logger::Close() {

}
