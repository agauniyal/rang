#ifndef RANG_DOT_HPP
#define RANG_DOT_HPP

#if defined(__unix__) || defined(__unix) || defined(__linux__)
#define OS_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define OS_WIN
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MAC
#else
#error Unknown Platform
#endif

#if defined(OS_LINUX) || defined(OS_MAC)
#include <unistd.h>
#elif defined(OS_WIN)
#include <io.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <type_traits>

namespace rang {

enum class style {
	reset     = 0,
	bold      = 1,
	dim       = 2,
	italic    = 3,
	underline = 4,
	blink     = 5,
	reversed  = 6,
	conceal   = 7,
	crossed   = 8
};

enum class fg {
	black   = 30,
	red     = 31,
	green   = 32,
	yellow  = 33,
	blue    = 34,
	magenta = 35,
	cyan    = 36,
	gray    = 37
};

enum class bg {
	black   = 40,
	red     = 41,
	green   = 42,
	yellow  = 43,
	blue    = 44,
	magenta = 45,
	cyan    = 46,
	gray    = 47
};

enum class fgB {
	black   = 90,
	red     = 91,
	green   = 92,
	yellow  = 93,
	blue    = 94,
	magenta = 95,
	cyan    = 96,
	gray    = 97
};

enum class bgB {
	black   = 100,
	red     = 101,
	green   = 102,
	yellow  = 103,
	blue    = 104,
	magenta = 105,
	cyan    = 106,
	gray    = 107
};

inline bool supportsColor()
{

#if defined(OS_LINUX) || defined(OS_MAC)
	const std::string Terms[] =
		{
			"ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm",
			"linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm"
		};

	const char *env_p = std::getenv("TERM");
	if (env_p == nullptr) {
		return false;
	}

	std::string env_string(env_p);
	static const bool result = std::any_of(std::begin(Terms), std::end(Terms),
		[&](std::string term) {
			return env_string.find(term) != std::string::npos;
		});

#elif defined(OS_WIN)
	static const bool result = true;
#endif

	return result;
}

inline bool isTerminal(const std::streambuf *osbuf)
{
	if (osbuf == std::cout.rdbuf()) {
#if defined(OS_LINUX) || defined(OS_MAC)
		return isatty(fileno(stdout)) ? true : false;
#elif defined(OS_WIN)
		return _isatty(_fileno(stdout)) ? true : false;
#endif
	}

	if (osbuf == std::cerr.rdbuf() || osbuf == std::clog.rdbuf()) {
#if defined(OS_LINUX) || defined(OS_MAC)
		return isatty(fileno(stderr)) ? true : false;
#elif defined(OS_WIN)
		return _isatty(_fileno(stderr)) ? true : false;
#endif
	}
	return false;
}


template <typename T>
using enable = typename std::enable_if
	<
		std::is_same<T, rang::style>::value ||
		std::is_same<T, rang::fg>::value ||
		std::is_same<T, rang::bg>::value ||
		std::is_same<T, rang::fgB>::value ||
		std::is_same<T, rang::bgB>::value,
		std::ostream&
	>::type;

template <typename T>
inline enable<T> operator<<(std::ostream &os, T const value)
{
	std::streambuf const *osbuf = os.rdbuf();
	return ((supportsColor()) && (isTerminal(osbuf)))
	  ? os << "\033[" << static_cast<int>(value) << "m"
	  : os;
}
}

#undef OS_LINUX
#undef OS_WIN
#undef OS_MAC

#endif /* ifndef RANG_DOT_HPP */
