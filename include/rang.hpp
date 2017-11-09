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
#include <cstring>

#elif defined(OS_WIN)

#if defined(__MINGW32__) || defined(__MINGW64__)
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <io.h>
#include <VersionHelpers.h>

// Only defined in windows 10 onwards, redefining in lower windows since it
// doesn't gets used in lower versions
// https://docs.microsoft.com/en-us/windows/console/getconsolemode
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>

namespace rang {

enum class style {
    reset     = 0,
    bold      = 1,
    dim       = 2,
    italic    = 3,
    underline = 4,
    blink     = 5,
    rblink    = 6,
    reversed  = 7,
    conceal   = 8,
    crossed   = 9
};

enum class fg {
    black   = 30,
    red     = 31,
    green   = 32,
    yellow  = 33,
    blue    = 34,
    magenta = 35,
    cyan    = 36,
    gray    = 37,
    reset   = 39
};

enum class bg {
    black   = 40,
    red     = 41,
    green   = 42,
    yellow  = 43,
    blue    = 44,
    magenta = 45,
    cyan    = 46,
    gray    = 47,
    reset   = 49
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

enum class control { offColor = 0, autoColor = 1, forceColor = 2 };


namespace rang_implementation {

    inline std::atomic<int> &controlValue() noexcept
    {
        static std::atomic<int> value(1);
        return value;
    }

    inline bool supportsColor() noexcept
    {
#if defined(OS_LINUX) || defined(OS_MAC)

        static const bool result = [] {
            const char *Terms[]
              = { "ansi",    "color",  "console", "cygwin", "gnome",
                  "konsole", "kterm",  "linux",   "msys",   "putty",
                  "rxvt",    "screen", "vt100",   "xterm" };

            const char *env_p = std::getenv("TERM");
            if (env_p == nullptr) {
                return false;
            } else {
                return std::any_of(
                  std::begin(Terms), std::end(Terms), [&](const char *term) {
                      return std::strstr(env_p, term) != nullptr;
                  });
            }
        }();

#elif defined(OS_WIN)
        // GetVersion() is deprecated in newer Visual Studio compilers
        static constexpr bool result = true;
#endif
        return result;
    }

#ifdef OS_WIN
    inline bool isMsysPty(int fd) noexcept
    {
        // Dynamic load for binary compability of old windows
        decltype(&GetFileInformationByHandleEx) ptrGetFileInformationByHandleEx = nullptr;
        if(!ptrGetFileInformationByHandleEx)
        {
            ptrGetFileInformationByHandleEx = 
            reinterpret_cast<decltype(&GetFileInformationByHandleEx)>(
                GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
                                "GetFileInformationByHandleEx"));
            if(!ptrGetFileInformationByHandleEx)
                return false;
        }
        constexpr const DWORD size = sizeof(FILE_NAME_INFO) + sizeof(WCHAR) * (MAX_PATH + 1);
        char buffer[size];
        FILE_NAME_INFO *nameinfo = reinterpret_cast<FILE_NAME_INFO*>(&buffer[0]);

        HANDLE h = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
        if (h == INVALID_HANDLE_VALUE)
            return false;
        // Check that it's a pipe:
        if (GetFileType(h) != FILE_TYPE_PIPE)
            return false;
        // Check pipe name is template of {"cygwin-","msys-"}XXXXXXXXXXXXXXX-ptyX-XX
        if(ptrGetFileInformationByHandleEx(h,FileNameInfo,nameinfo,size-sizeof(WCHAR))) {
            nameinfo->FileName[nameinfo->FileNameLength / sizeof(WCHAR)] = L'\0';
            PWSTR name = nameinfo->FileName;
            if ((!wcsstr(name, L"msys-") && !wcsstr(name, L"cygwin-")) ||
                 !wcsstr(name, L"-pty"))
                return false;
        }
        return true;
    }
#endif

    inline bool isTerminal(const std::streambuf *osbuf) noexcept
    {
        using std::cerr;
        using std::clog;
        using std::cout;
#if defined(OS_LINUX) || defined(OS_MAC)
        if (osbuf == cout.rdbuf()) {
            static bool cout_term = isatty(fileno(stdout)) ? true : false;
            return cout_term;
        } else if (osbuf == cerr.rdbuf() || osbuf == clog.rdbuf()) {
            static bool cerr_term = isatty(fileno(stderr)) ? true : false;
            return cerr_term;
        }
#elif defined(OS_WIN)
        if (osbuf == cout.rdbuf()) {
            static bool cout_term = ( _isatty(_fileno(stdout)) || 
                                        isMsysPty(_fileno(stdout)) ) ? true : false;
            return cout_term;
        } else if (osbuf == cerr.rdbuf() || osbuf == clog.rdbuf()) {
            static bool cerr_term = ( _isatty(_fileno(stderr)) || 
                                        isMsysPty(_fileno(stderr)) ) ? true : false;
            return cerr_term;
        }
#endif
        return false;
    }

    template <typename T>
    using enableStd = typename std::enable_if<
      std::is_same<T, rang::style>::value || std::is_same<T, rang::fg>::value
        || std::is_same<T, rang::bg>::value || std::is_same<T, rang::fgB>::value
        || std::is_same<T, rang::bgB>::value,
      std::ostream &>::type;


#ifdef OS_WIN

    inline HANDLE getConsoleHandle() noexcept
    {
        static const HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        return h;
    }

    inline bool setWinTermAnsiColors() noexcept
    {
        HANDLE h = getConsoleHandle();
        if (h == INVALID_HANDLE_VALUE) {
            return false;
        }
        if (IsWindowsVersionOrGreater(10, 0, 0)) {
            DWORD dwMode = 0;
            if (!GetConsoleMode(h, &dwMode)) {
                return false;
            }
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(h, dwMode)) {
                return false;
            }
            return true;
        } else {
            return false;
        }
    }

    inline bool supportsAnsi(const std::streambuf *osbuf) noexcept
    {
        using std::cerr;
        using std::clog;
        using std::cout;
        static const bool nativeAnsi = setWinTermAnsiColors();
        if (osbuf == cout.rdbuf()) {
            static bool cout_ansi = (isMsysPty(_fileno(stdout)) || nativeAnsi) ? true : false;
            return cout_ansi;
        } else if (osbuf == cerr.rdbuf() || osbuf == clog.rdbuf()) {
            static bool cerr_ansi = (isMsysPty(_fileno(stderr)) || nativeAnsi) ? true : false;
            return cerr_ansi;
        }
        return false;
    }

    inline WORD defaultState() noexcept
    {
        static const WORD defaultAttributes = []() -> WORD {
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(getConsoleHandle(), &info))
                return (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
            return info.wAttributes;
        }();
        return defaultAttributes;
    }

    inline WORD reverseRGB(WORD rgb) noexcept
    {
        static const WORD rev[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
        return rev[rgb];
    }

    inline void setWinAttribute(rang::bg col, WORD &state) noexcept
    {
        state &= 0xFF0F;
        if (col != rang::bg::reset) {
            state |= reverseRGB(static_cast<WORD>(col) - 40) << 4;
        } else {
            state |= (defaultState() & 0xF0);
        }
    }

    inline void setWinAttribute(rang::fg col, WORD &state) noexcept
    {
        state &= 0xFFF0;
        if (col != rang::fg::reset) {
            state |= reverseRGB(static_cast<WORD>(col) - 30);
        } else {
            state |= (defaultState() & 0x0F);
        }
    }

    inline void setWinAttribute(rang::bgB col, WORD &state) noexcept
    {
        state &= 0xFF0F;
        state |= (0x8 | reverseRGB(static_cast<WORD>(col) - 100)) << 4;
    }

    inline void setWinAttribute(rang::fgB col, WORD &state) noexcept
    {
        state &= 0xFFF0;
        state |= (0x8 | reverseRGB(static_cast<WORD>(col) - 90));
    }

    inline void setWinAttribute(rang::style style, WORD &state) noexcept
    {
        if (style == rang::style::reset) {
            state = defaultState();
        }
    }

    inline WORD &current_state() noexcept
    {
        static WORD state = defaultState();
        return state;
    }

    template <typename T>
    inline enableStd<T> setColor(std::ostream &os, T const value)
    {
        if (supportsAnsi(os.rdbuf())) {
            return os << "\033[" << static_cast<int>(value) << "m";
        } else {
            const HANDLE h = getConsoleHandle();
            if (h != INVALID_HANDLE_VALUE && isTerminal(os.rdbuf())) {
                setWinAttribute(value, current_state());
                SetConsoleTextAttribute(h, current_state());
            }
            return os;
        }
    }
#else
    template <typename T>
    inline enableStd<T> setColor(std::ostream &os, T const value)
    {
        return os << "\033[" << static_cast<int>(value) << "m";
    }
#endif
}  // namespace rang_implementation

template <typename T>
inline rang_implementation::enableStd<T> operator<<(std::ostream &os,
                                                    const T value)
{
    const int option = rang_implementation::controlValue();
    switch (option) {
        case 0: return os;
        case 1:
            return rang_implementation::supportsColor()
                && rang_implementation::isTerminal(os.rdbuf())
              ? rang_implementation::setColor(os, value)
              : os;
        case 2: return rang_implementation::setColor(os, value);
        default: return os;
    }
}

inline std::ostream &operator<<(std::ostream &os, const rang::control value)
{
    if (value == rang::control::offColor) {
        rang_implementation::controlValue() = 0;
    } else if (value == rang::control::autoColor) {
        rang_implementation::controlValue() = 1;
    } else if (value == rang::control::forceColor) {
        rang_implementation::controlValue() = 2;
    }
    return os;
}
}  // namespace rang

#undef OS_LINUX
#undef OS_WIN
#undef OS_MAC

#endif /* ifndef RANG_DOT_HPP */
