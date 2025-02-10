#pragma once

#include <mutex>
#include <iostream>

#define tcpft_logInfo(...)      Logger::Instance().Log("[" __FUNCTION__ "][INF]: ", __VA_ARGS__, "\n");
#define tcpft_logWarning(...)   Logger::Instance().Log("[" __FUNCTION__ "][WRN]: ", __VA_ARGS__, "\n");
#define tcpft_logCritical(...)  Logger::Instance().Log("[" __FUNCTION__ "][CRT]: ", __VA_ARGS__, "\n");
#define tcpft_logFatal(...)     Logger::Instance().Log("[" __FUNCTION__ "][FTL]: ", __VA_ARGS__, "\n");

/**
 * @brief Singleton logger for thread-safe logging.
 */
class Logger {
public:
    /**
     * @brief Returns the singleton instance of Logger.
     *
     * @return Logger& reference to the Logger instance.
     */
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

     /**
     * @brief Logs a variable number of arguments to std::cout.
     *
     * @tparam Args Parameter pack types.
     * @param args Values to be logged.
     */
    template <typename... Args>
    void Log(Args... args) {
#ifdef TCPFT_LOG_ENABLE
        std::lock_guard<std::mutex> lock(_mutex);
        // Using initializer list trick to unpack the parameter pack:
        using expander = int[];
        (void)expander {
            0, ((std::cout << args), 0)...
        };
#endif // TCPFT_LOG_ENABLE
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::mutex _mutex;
};