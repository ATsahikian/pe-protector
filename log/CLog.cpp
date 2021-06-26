#include "CLog.h"
#include <stdarg.h>
#include <time.h>
#include <sstream>
#include <string>
#include <thread>

using std::string;

namespace {
string timeToString(const time_t& time) {
  char buffer[80];

  strftime(buffer, 80, "%c", localtime(&time));

  return string(buffer);
}
}  // namespace

CLog& CLog::getInstance() {
  static CLog log;
  return log;
}

bool CLog::initialize(const string& fileName) {
  mFileHandle = fopen(fileName.c_str(), "w");
  return true;
}

void CLog::log(const string& type, const char* const format, ...) {
  va_list args;
  va_start(args, format);

  if (mFileHandle != 0) {
    std::stringstream ss;
    ss << std::this_thread::get_id();

    fprintf(mFileHandle, "[%s] [%s] [%s] : ", timeToString(time(0)).c_str(),
            ss.str().c_str(), type.c_str());

    vfprintf(mFileHandle, format, args);

    fprintf(mFileHandle, "\n");

    fflush(mFileHandle);
  }

  va_end(args);
}

CLog::CLog() : mFileHandle() {}

CLog::~CLog() {
  if (mFileHandle != 0) {
    fclose(mFileHandle);
  }
}
