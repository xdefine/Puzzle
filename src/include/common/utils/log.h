#ifndef COMMON_UTILS_LOG_H
#define COMMON_UTILS_LOG_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

#include "common/utils/config.h"

namespace puzzle {

class Log {
public:
    ~Log();

    static void AppendLog(const char *log, Size len);
private:
    Log();

    static Log &GetInstance();

    constexpr const static char *kLogFilePath = "run.log";
    int log_fd_;

    const static Size kLogBufferSize = 8192;
    char *append_buffer_;
    Size append_index_;

    std::vector<std::pair<char *, Size>> flush_queue_;
    std::thread flush_thread_;
    std::mutex mutex_;
    std::condition_variable notify_;

    std::atomic<bool> running_;
    void FlushBuffersToLogFile();
};

}  // namespace puzzle

#endif  // COMMON_UTILS_LOG_H