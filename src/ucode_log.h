#pragma once
#include <fstream>
#include <sstream>

static constexpr const char *LOG_FILE_NAME = "ucode.log";

// TODO: define to noting in debug mode
#define Debug LogMessage()

void LogInit();

struct LogMessage: public std::ostringstream {
    
    // TODO: is this slow?
    ~LogMessage();
};

