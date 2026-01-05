#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstdio>
#include <chrono>
#include <mutex>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG,
    INPUT,
    CRITICAL // ,
    // TRACE
};

enum class LogCategory 
{
    CORE,
    RENDERING,
    OTHER
};

struct LogData
{
    LogLevel lvl;
    LogCategory cat;

    const std::string m;
    const std::string f;
    int line;

    std::chrono::system_clock::time_point timestamp;
};

class ILogSink
{
public:
    virtual ~ILogSink() = default;
    virtual void write(const LogData& data) = 0;
};

class Logger
{
private:
    static std::vector<ILogSink*>& sinks() 
    {
        static std::vector<ILogSink*> instance;
        return instance;
    }

    static std::mutex& logMutex() 
    {
        static std::mutex instance;
        return instance;
    }

public:
    static void AddSink(ILogSink* sink)
    {
        std::lock_guard<std::mutex> lock(logMutex());

        sinks().push_back(sink);
    }

    static void Log(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex());

        LogData data{level, LogCategory::OTHER, message, __FILE__, __LINE__, std::chrono::system_clock::now()};
        for (auto* sink : sinks())
            sink->write(data);
    }

    static void Log(LogLevel level, LogCategory cat, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex());
        
        LogData data{level, cat, message, __FILE__, __LINE__, std::chrono::system_clock::now()};
        for (auto* sink : sinks())
            sink->write(data);
    }

};

class ConsoleLogger : public ILogSink
{
public:
    void write(const LogData& data) override
    {
        const char* lvl = levelToString(data.lvl);

        std::printf("[%s] %s\n", lvl, data.m.c_str());
    }

private:
    static const char* levelToString(LogLevel lvl)
    {
        switch (lvl) 
        {
            case LogLevel::INFO:     return "INFO";
            case LogLevel::WARNING:  return "WARNING";
            case LogLevel::ERROR:    return "ERROR";
            case LogLevel::DEBUG:    return "DEBUG";
            case LogLevel::INPUT:    return "INPUT";
            case LogLevel::CRITICAL: return "CRITICAL";
            default:                 return "UNKNOWN";
        }
    }
};

#endif // LOGGER_H