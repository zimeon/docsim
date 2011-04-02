#ifndef __INC_Logger
#define __INC_Logger 1

#include <fstream>

class Logger
{
public:
  //DATA//
  std::ofstream log;
  bool logToCout;
  std::ofstream warn;
  bool warnToCout;

  //METHODS//
  Logger();
  ~Logger(void);
  void init(const char* logFile, bool ltc, const char* warnFile, bool wtc);


};

#endif /* #ifndef __INC_Logger */
