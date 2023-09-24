#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#include "common/utils/assert.h"
#include "common/utils/log.h"

namespace puzzle {

Log::Log() {
    log_fd_ = open(kLogFilePath, O_APPEND | O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
    if (log_fd_ == -1) {
        std::cout << "Log::Open log file error: " << strerror(errno) << std::endl;
        exit(-1);
    }

    append_buffer_ = new char[kLogBufferSize];
    append_index_ = 0;

    running_ = true;
    flush_thread_ = std::thread(&Log::FlushBuffersToLogFile, this);
}

Log::~Log() {

}

Log &Log::GetInstance() {
    static Log log;
    return log;
}

void Log::AppendLog(const char *log, Size len) {
    Log &log_instance = Log::GetInstance();
    Assert(len <= kLogBufferSize, "Log length should less than kLogBufferSize");
    std::lock_guard<std::mutex> lock(log_instance.mutex_);
    if (log_instance.append_index_ + len > kLogBufferSize) {
        log_instance.flush_queue_.emplace_back(log_instance.append_buffer_, log_instance.append_index_);
        log_instance.append_buffer_ = new char[kLogBufferSize];
        log_instance.append_index_ = 0;
    }
    memcpy(log_instance.append_buffer_ + log_instance.append_index_, log, len);
    log_instance.append_index_ += len;
    if (log_instance.append_index_ == len) {
        log_instance.notify_.notify_one();
    }
}

void Log::Stop() {

}

void Log::FlushBuffersToLogFile() {
    while (running_) {
        std::vector<std::pair<char *, Size>> flush_queue;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (flush_queue_.empty() && append_index_ == 0) {
                notify_.wait_for(lock, std::chrono::microseconds(1000));
                continue;
            }
            if (append_index_ != 0) {
                flush_queue_.emplace_back(append_buffer_, append_index_);
                append_buffer_ = new char[kLogBufferSize];
                append_index_ = 0;
            }
            flush_queue.swap(flush_queue_);
        }
        for (std::pair<char *, Size> &p : flush_queue) {
            Size write_size = write(log_fd_, p.first, p.second);
            Assert(write_size == p.second);
        }
    }

}

}