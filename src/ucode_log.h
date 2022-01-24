#pragma once
#include <fstream>
#include <sstream>

static constexpr const char *LOG_FILE_NAME = "ucode.log";

// TODO: define to noting in debug mode
#define Debug LogMessage()

void LogInit() {
    std::ofstream fp(LOG_FILE_NAME, std::ios::out);
}

struct LogMessage: public std::ostringstream {
    
    // TODO: is this slow?
    ~LogMessage() {
        std::ofstream fp(LOG_FILE_NAME, std::ios::out | std::ios::app);
        fp << this->str().c_str() << std::endl;
    }
};

