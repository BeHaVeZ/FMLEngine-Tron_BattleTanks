#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <memory>

namespace FML
{

	enum class LogLevel
	{
		Debug,
		Info,
		Warning,
		Error
	};

	class Logger
	{
	public:
		static void Log(LogLevel level, const std::string& message)
		{
			std::string colorCode;
			switch (level) {
			case LogLevel::Debug:
				colorCode = "\033[36m";
				break;
			case LogLevel::Info:
				colorCode = "\033[32m";
				break;
			case LogLevel::Warning:
				colorCode = "\033[33m";
				break;
			case LogLevel::Error:
				colorCode = "\033[31m";
				break;
			}
			std::cout << colorCode << "[" << ToString(level) << "]: " << message << "\033[0m" << std::endl;
		}

		template<typename... Args>
		static void Log(LogLevel level, const std::string& format, Args... args) {
			std::string formattedMessage = Format(format, args...);
			Log(level, formattedMessage);
		}

	private:
		static std::string ToString(LogLevel level)
		{
			switch (level)
			{
			case LogLevel::Debug: return "DEBUG";
			case LogLevel::Info: return "INFO";
			case LogLevel::Warning: return "WARNING";
			case LogLevel::Error: return "ERROR";
			default: return "[UNKNOWN]";
			}
		}

		template<typename... Args>
		static std::string Format(const std::string& format, Args... args) {
			size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
			std::unique_ptr<char[]> buf(new char[size]);
			snprintf(buf.get(), size, format.c_str(), args...);
			return std::string(buf.get(), buf.get() + size - 1);
		}
	};
	/*
	USAGE:
	Logger::Log(LogLevel::Info, "Formatted number: %d", 25);
	Logger::Log(LogLevel::Info, "Fixed point: %.2f", 3.14159);
	Logger::Log(LogLevel::Info, "Scientific: %.3e", 3.14159);
	Logger::Log(LogLevel::Info, "Hex: %#x", 255);
	Logger::Log(LogLevel::Info, "Octal: %#o", 123);
	Logger::Log(LogLevel::Info, "Padded Number: %05d", 25);
	Logger::Log(LogLevel::Info, "Width and left justify: %-10dend", 25);
	Logger::Log(LogLevel::Info, "Show sign: %+d", 25);
	OUTPUT:
	[INFO] : Formatted number : 25
	[INFO] : Fixed point : 3.14
	[INFO] : Scientific : 3.142e+00
	[INFO] : Hex : 0xff
	[INFO] : Octal : 0173
	[INFO] : Padded Number : 00025
	[INFO] : Width and left justify : 25        end
	[INFO] : Show sign : +25
	*/
}