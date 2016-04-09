#pragma once

#if defined(TRACE)
#undef TRACE
#endif

#include <cstdarg>
#include <cstdio>

// #pragma warning (disable:4786)

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __WARN__ __FILE__ "("__STR1__(__LINE__)"): Warning: "
#define __TODO__ __FILE__ "("__STR1__(__LINE__)"): TODO: "

#ifndef NDEBUG

#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <cstring>
#include <cstdio>

#ifdef _WINDOWS_
  #ifndef _CRT_SECURE_NO_DEPRECATE
  #define _CRT_SECURE_NO_DEPRECATE
  #endif
  #include <crtdbg.h>
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #include <unistd.h>
#endif

inline void __xeno__trace__(const char* format,...)
{
  char buffer[81];
  va_list args;
  va_start(args, format);
  size_t buflen = ::vsnprintf(buffer, sizeof(buffer), format, args);
  if (buflen >= sizeof(buffer)) {
	  ::strcpy(buffer+sizeof(buffer)-5, "...\n");
	  buflen = sizeof(buffer)-1;
  }
#ifdef _WINDOWS_
  OutputDebugString(buffer);
#else
  write(STDERR_FILENO, buffer, buflen);
#endif
}

#define TRACE __xeno__trace__
#define TRACELINE(x) (TRACE(x),TRACE("\n"))
#define TRACELN(x) TRACELINE(x)

#else

void inline __xeno__trace__(...) {}
#define TRACE if (false) __xeno__trace__
#define TRACELINE(x)
#define TRACELN(x)

#endif

void inline __xeno__trace__force__(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
}
#define TRACE_FORCE __xeno__trace__force__
