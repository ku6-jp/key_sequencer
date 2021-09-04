#include <Windows.h>
#include <tchar.h>

#include <cstdio>

namespace keysequencer {

void debug(const TCHAR* message, ...) {
  TCHAR buffer[1024];

  va_list arguments;
  va_start(arguments, message);
  int length = _vstprintf_s(buffer, 1021, message, arguments);
  va_end(arguments);

  if (0 <= length) {
    buffer[length] = '\r';
    buffer[length + 1] = '\n';
    buffer[length + 2] = 0;

    OutputDebugString(buffer);
  }
}

}  // namespace keysequencer