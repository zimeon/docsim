#include "Logger.h"
using namespace std;

Logger::Logger(void)
{
}

Logger::~Logger(void)
{
  if (log!=NULL) log.close();  
  if (warn!=NULL) warn.close();
}

void Logger::init(const char* logFile, bool ltc, const char* warnFile, bool wtc)
{
  log.open(logFile, ios_base::out);
  logToCout=ltc;
  warn.open(warnFile, ios_base::out);
  warnToCout=wtc;
}
