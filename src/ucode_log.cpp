#include "ucode_log.h"

void LogInit() {
    std::ofstream fp(LOG_FILE_NAME, std::ios::out);
}

LogMessage::~LogMessage() {
    std::ofstream fp(LOG_FILE_NAME, std::ios::out | std::ios::app);
    fp << this->str().c_str() << std::endl;
}

