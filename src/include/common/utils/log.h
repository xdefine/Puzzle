#ifndef COMMON_UTILS_LOG_H
#define COMMON_UTILS_LOG_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "common/utils/config.h"

namespace puzzle {

enum class Module {
    Common,
};

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

#define LOG_DEBUG(module, format, ...)                                                                                 \
    Log::OutputLog(module, FILE_RELATIVE_PATH, __LINE__, LogLevel::DEBUG, format, ##__VA_ARGS__)

#define LOG_INFO(module, format, ...)                                                                                  \
    Log::OutputLog(module, FILE_RELATIVE_PATH, __LINE__, LogLevel::INFO, format, ##__VA_ARGS__)

#define LOG_WARN(module, format, ...)                                                                                  \
    Log::OutputLog(module, FILE_RELATIVE_PATH, __LINE__, LogLevel::WARN, format, ##__VA_ARGS__)

#define LOG_ERROR(module, format, ...)                                                                                 \
    Log::OutputLog(module, FILE_RELATIVE_PATH, __LINE__, LogLevel::ERROR, format, ##__VA_ARGS__)

/**
 *
 */
class LogBuffer {
public:
    bool Append(const char *log, Size len);
    LogBuffer();
    LogBuffer(LogBuffer &other) = delete;
    LogBuffer(LogBuffer &&other) noexcept;

    LogBuffer &operator=(LogBuffer &other) = delete;
    LogBuffer &operator=(LogBuffer &&other) = delete;
    ~LogBuffer();

    void WriteToFd(int fd);
    Size GetLength() const;

private:
    friend void swap(LogBuffer &lhs, LogBuffer &rhs);
    const static Size kLogBufferSize = 8192;

    char *buffer_;
    Size len_;
};

void swap(LogBuffer &lhs, LogBuffer &rhs) {
    std::swap(lhs.buffer_, rhs.buffer_);
    std::swap(lhs.len_, rhs.len_);
}


class Log {
public:
    ~Log() = default;

    static void OutputLog(Module module, const char *file, int line, LogLevel level, const char *format, ...);
    static void StopAndWaitFlushComplete();

private:
    constexpr const static char *kLogFilePath = "run.log";
    const static Size kMaxSingleLogBodySize = 512;
    int log_fd_;

    LogBuffer buffer_;

    std::vector<LogBuffer> flush_queue_;
    std::thread flush_thread_;
    std::mutex mutex_;
    std::condition_variable notify_;

    constexpr static std::chrono::microseconds kInitSleepTime {500};
    constexpr static std::chrono::microseconds kMaxSleepTime {5000};
    std::chrono::microseconds sleep_time;

    std::atomic<bool> keep_running_;
    void FlushBuffersToLogFile();

private:
    Log();
    static std::stringstream GenerateLogHeader(Module module, const char *file, int line, LogLevel level);
    void AppendLog(const char *log, Size len);
    static Log &GetInstance();
};

}  // namespace puzzle

#endif  // COMMON_UTILS_LOG_H