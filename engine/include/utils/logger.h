#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <format>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// DEMO-------------------------------------------------------------------------------
// Logger::LoggerInstance::GetInstance().SetVerbosity(Logger::LogLevel::DEBUG);
// Logger::LoggerInstance::GetInstance().SetLogFile("log/game_log.txt");
// Logger::LogDebug("This is a debug message.");
// Logger::LogInfo("This is an info message.");
// Logger::LogWarning("This is a warning message.");
// Logger::LogError("This is an error message.");
// Log(Logger::LogLevel::DEBUG, "This is a debug message.");
// Log(Logger::LogLevel::INFO, "This is an info message.");
// Log(Logger::LogLevel::WARNING, "This is a warning message.");
// Log(Logger::LogLevel::ERR, "This is an error message.");
// -----------------------------------------------------------------------------------

namespace Logger
{
    enum class LogLevel
    {
        NONE,
        ERR,
        WARNING,
        INFO,
        DEBUG
    };

    // Log Sink Interface
    class LogSink
    {
    public:
        virtual ~LogSink() = default;
        virtual void Write(LogLevel level, const std::string &message) = 0;
    };

    // Console Sink Interface
    class ConsoleSink : public LogSink
    {
    public:
        void Write(LogLevel level, const std::string &message) override;
    };

    // File Sink Interface
    class FileSink : public LogSink
    {
    public:
        explicit FileSink(const std::string &filename);
        void Write(LogLevel level, const std::string &message) override;

    private:
        void EnsureDirectoryExists(const std::string &path);

        std::ofstream m_log_file;
        std::mutex m_file_mutex;
        bool m_first_write = true;
    };

    // Logger Instance
    class LoggerInstance
    {
    public:
        static LoggerInstance &GetInstance();

        void SetVerbosity(LogLevel level);
        void SetLogFile(const std::string &filename);

        template <typename... Args>
        void Log(LogLevel level, std::format_string<Args...> fmt, Args &&...args)
        {
            if (level > m_verbosity)
                return;

            std::string message = std::format(fmt, std::forward<Args>(args)...);
            WriteToSinks(level, message);
        }

    private:
        LogLevel m_verbosity = LogLevel::INFO;
        std::vector<std::shared_ptr<LogSink>> m_sinks;
        std::mutex m_mutex;
        bool m_has_file_sink = false;

        LoggerInstance();
        void WriteToSinks(LogLevel level, const std::string &message);
    };

    // Global APIs
    template <typename... Args>
    inline void Log(LogLevel level, std::format_string<Args...> fmt, Args &&...args)
    {
        LoggerInstance::GetInstance().Log(level, fmt, std::forward<Args>(args)...);
    }

    inline void LogError(const std::string &message) { Log(LogLevel::ERR, "{}", message); }
    inline void LogWarning(const std::string &message) { Log(LogLevel::WARNING, "{}", message); }
    inline void LogInfo(const std::string &message) { Log(LogLevel::INFO, "{}", message); }
    inline void LogDebug(const std::string &message) { Log(LogLevel::DEBUG, "{}", message); }

} // namespace Logger