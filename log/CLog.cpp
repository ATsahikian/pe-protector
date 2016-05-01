#include "CLog.h"

#include <stdarg.h>
#include <time.h>
#include <sstream>
#include <string>
#include <thread>

namespace {
std::string timeToString(const time_t& time) {
  char buffer[80];

  strftime(buffer, 80, "%c", localtime(&time));

  return std::string(buffer);
}
}  // namespace

CLog& CLog::getInstance() {
  static CLog log;
  return log;
}

bool CLog::initialize(const std::string& fileName) {
  // mFileHandle = fopen(fileName.c_str(), "w");
  return true;
}

void CLog::log(const std::string& type, const char* const format, ...) {
  va_list args;
  va_start(args, format);

  // if (mFileHandle != 0) {
  std::stringstream ss;
  ss << std::this_thread::get_id();

  fprintf(stderr, "[%s] [%s] [%s] : ", timeToString(time(0)).c_str(),
          ss.str().c_str(), type.c_str());

  vfprintf(stderr, format, args);

  fprintf(stderr, "\n");

  fflush(stderr);
  //}

  va_end(args);
}

CLog::CLog() : mFileHandle() {}

CLog::~CLog() {
  if (mFileHandle != 0) {
    fclose(mFileHandle);
  }
}
