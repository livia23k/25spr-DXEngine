#include "utils/logger.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace Logger
{
    static std::string GetCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm local_time{};
        localtime_s(&local_time, &time_t_now);

        std::ostringstream ss;
        ss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // ConsoleSink Impl
    void ConsoleSink::Write(LogLevel level, const std::string &message)
    {
        // color code
        const char *color_code = "";
        switch (level)
        {
        case LogLevel::ERR:
            color_code = "\033[31m"; // red
            break;
        case LogLevel::WARNING:
            color_code = "\033[33m"; // yellow
            break;
        case LogLevel::INFO:
            color_code = "\033[37m"; // white
            break;
        case LogLevel::DEBUG:
            color_code = "\033[36m"; // cyan
            break;
        default:
            break;
        }
        static const char *level_names[] = {"", "[ERROR] ", "[WARN ] ", "[INFO ] ", "[DEBUG] "};
        std::cout << std::format("[{}] {}{}{}\033[0m\n", GetCurrentTime(), color_code, level_names[static_cast<int>(level)], message);
    }

    // FileSink Impl
    FileSink::FileSink(const std::string &filename)
    {
        std::filesystem::path filePath(filename);
        std::filesystem::path dirPath = filePath.parent_path();

        if (!dirPath.empty() && !std::filesystem::exists(dirPath))
        {
            std::filesystem::create_directories(dirPath);
        }

        m_log_file.open(filename, std::ios::app);
        if (!m_log_file.is_open())
        {
            throw std::runtime_error("FileSink::FileSink: Failed to open log file: " + filename);
        }
    }

    void FileSink::Write(LogLevel level, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(m_file_mutex);
        if (m_log_file.is_open())
        {
            if (m_first_write)
            {
                m_log_file << "----- Log Start -----\n";
                m_first_write = false;
            }

            static const char *level_names[] = {"", "[ERROR] ", "[WARN ] ", "[INFO ] ", "[DEBUG] "};
            m_log_file << std::format("[{}] {}{}\n", GetCurrentTime(), level_names[static_cast<int>(level)], message);
        }
    }

    // LoggerInstance Impl
    LoggerInstance::LoggerInstance()
    {
        m_sinks.push_back(std::make_shared<ConsoleSink>());
    }

    LoggerInstance &LoggerInstance::GetInstance()
    {
        static LoggerInstance instance;
        return instance;
    }

    void LoggerInstance::SetVerbosity(LogLevel level)
    {
        m_verbosity = level;
    }

    void LoggerInstance::SetLogFile(const std::string &filename)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_has_file_sink)
        {
            m_sinks.push_back(std::make_shared<FileSink>(filename));
            m_has_file_sink = true;
        }
    }

    void LoggerInstance::WriteToSinks(LogLevel level, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto &sink : m_sinks)
        {
            sink->Write(level, message);
        }
    }

} // namespace Logger