#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <unistd.h>

#include "common/utils/assert.h"
#include "common/utils/log.h"

namespace puzzle {

LogBuffer::LogBuffer() : buffer_(nullptr), len_(0) {
    buffer_ = new char[kLogBufferSize];
}

LogBuffer::~LogBuffer() {
    delete buffer_;
    buffer_ = nullptr;
}

LogBuffer::LogBuffer(LogBuffer &&other) noexcept {
    buffer_ = other.buffer_;
    len_ = other.len_;

    other.buffer_ = nullptr;
    other.len_ = 0;
}

bool LogBuffer::Append(const char *log, Size len) {
    if (len_ + len > kLogBufferSize) {
        return false;
    }
    memcpy(buffer_ + len_, log, len);
    len_ += len;
    return true;
}

void LogBuffer::WriteToFd(int fd) {
    if (len_ == 0) {
        return;
    }
    Assert(fd != -1);
    Size write_len = write(fd, buffer_, len_);
    Assert(write_len == len_);
    Assert(fsync(fd) == 0);
}

Size LogBuffer::GetLength() const {
    return len_;
}

Log::Log() : log_fd_(-1), sleep_time(kInitSleepTime), keep_running_(true) {
    log_fd_ = open(kLogFilePath, O_APPEND | O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
    if (log_fd_ == -1) {
        std::cout << "Log::Open log file error: " << strerror(errno) << std::endl;
        exit(-1);
    }

    flush_thread_ = std::thread(&Log::FlushBuffersToLogFile, this);
}

Log &Log::GetInstance() {
    static Log log;
    return log;
}

void Log::OutputLog(Module module, const char *file, int line, LogLevel level, const char *format, ...) {
    std::lock_guard<std::mutex> lock(GetInstance().mutex_);
    std::stringstream log = GenerateLogHeader(module, file, line, level);

    char log_body[kMaxSingleLogBodySize] = { 0 };
    ::va_list args;
    ::va_start(args, format);
    ::vsprintf(log_body, format, args);
    ::va_end(args);

    log << log_body << "\n";
    GetInstance().AppendLog(log.str().c_str(), log.str().length());
}

void Log::AppendLog(const char *log, Size len) {
    if (buffer_.Append(log, len)) {
        return;
    }
    LogBuffer t_buffer;
    swap(t_buffer, buffer_);
    flush_queue_.emplace_back(std::move(t_buffer));
    notify_.notify_one();
}

void Log::StopAndWaitFlushComplete() {
    GetInstance().keep_running_ = false;
    GetInstance().notify_.notify_one();
    GetInstance().flush_thread_.join();
}

void Log::FlushBuffersToLogFile() {
    while (true) {
        std::vector<LogBuffer> flush_queue;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (flush_queue_.empty() && buffer_.GetLength() == 0) {
                if (!keep_running_) {
                    break;
                }
                notify_.wait_for(lock, sleep_time);
                sleep_time = min(sleep_time * 2, kMaxSleepTime);
                continue;
            }
            sleep_time = kInitSleepTime;
            if (buffer_.GetLength()) {
                LogBuffer t_buffer;
                swap(t_buffer, buffer_);
                flush_queue_.emplace_back(std::move(t_buffer));
            }
            flush_queue.swap(flush_queue_);
        }
        for (LogBuffer &buffer : flush_queue) {
            buffer.WriteToFd(log_fd_);
        }
    }
}

std::stringstream Log::GenerateLogHeader(Module module, const char *file, int line, LogLevel level) {
    time_t t = ::time(nullptr);
    tm *curTime = localtime(&t);
    char time_str[32];
    ::strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", curTime);

    const char *module_name;
    switch (module) {
        case Module::Common:
            module_name = "Common";
            break;
        default:
            module_name = "UNKNOWN";
    }

    const char *type_name;
    switch (level) {
        case LogLevel::ERROR:
            type_name = "ERROR";
            break;
        case LogLevel::WARN:
            type_name = "WARN";
            break;
        case LogLevel::INFO:
            type_name = "INFO";
            break;
        case LogLevel::DEBUG:
            type_name = "DEBUG";
            break;
        default:
            type_name = "UNKNOWN";
            break;
    }

    std::stringstream log_header;
    log_header << "[ " << std::setw(6) << module_name << " : "
               << std::setw(5) << type_name << " ]"
               << " " << time_str << " "
               << "[ " << std::setw(30)  << std::setiosflags(std::ios::left) << file << ":"
               << std::setw(5) << std::setiosflags(std::ios::right) << line << " ] - ";
    return log_header;
}

}  // namespace puzzle