#ifndef HOROVOD_LOGGING_H
#define HOROVOD_LOGGING_H

#include <sstream>
#include <string>

namespace horovod {
namespace common {

/**
 * 旧版本 enum 存在的问题：
 * - 会发生整形的隐式转换
 * - 无法指定底层使用的数据类型
 * - 作用域问题，成员会暴露到上一级作用域中，从而导致不同的枚举体不能使用相同成员
 * - 编译器实现方法不统一
 * c++11 enum class/struct (强枚举类型，enum class 和 enum stuct 等价) 特点:
 * - 不会发生隐式类型转换，但是可以强转
 * - 底层数据类型可以指定
 * - 枚举体具有封装性，需要通过域运算符进行访问
*/
enum class LogLevel {
  TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
};

#define LOG_LEVELS "TDIWEF"

class LogMessage : public std::basic_ostringstream<char> {
 public:
  LogMessage(const char* fname, int line, LogLevel severity);
  ~LogMessage();

 protected:
  void GenerateLogMessage(bool log_time);

 private:
  const char* fname_;
  int line_;
  LogLevel severity_;
};

// LogMessageFatal ensures the process will exit in failure after
// logging this message.
class LogMessageFatal : public LogMessage {
 public:
  LogMessageFatal(const char* file, int line);
  ~LogMessageFatal();
};

#define _HVD_LOG_TRACE \
  LogMessage(__FILE__, __LINE__, LogLevel::TRACE)
#define _HVD_LOG_DEBUG \
  LogMessage(__FILE__, __LINE__, LogLevel::DEBUG)
#define _HVD_LOG_INFO \
  LogMessage(__FILE__, __LINE__, LogLevel::INFO)
#define _HVD_LOG_WARNING \
  LogMessage(__FILE__, __LINE__, LogLevel::WARNING)
#define _HVD_LOG_ERROR \
  LogMessage(__FILE__, __LINE__, LogLevel::ERROR)
#define _HVD_LOG_FATAL \
  LogMessageFatal(__FILE__, __LINE__)

#define _LOG(severity) _HVD_LOG_##severity

#define _LOG_RANK(severity, rank) _HVD_LOG_##severity << "[" << rank << "]: "

#define GET_LOG(_1, _2, NAME, ...) NAME
#define LOG(...) GET_LOG(__VA_ARGS__, _LOG_RANK, _LOG)(__VA_ARGS__)

LogLevel MinLogLevelFromEnv();
bool LogTimeFromEnv();

}
}

#endif // HOROVOD_LOGGING_H
