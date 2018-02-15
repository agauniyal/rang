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

#if defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0600)
#error                                                                         \
  "Please include rang.hpp before any windows system headers or set _WIN32_WINNT at least to _WIN32_WINNT_VISTA"
#elif !defined(_WIN32_WINNT)
#define _WIN32_WINNT _WIN32_WINNT_VISTA
#endif

#include <windows.h>
#include <io.h>
#include <memory>

// Only defined in windows 10 onwards, redefining in lower windows since it
// doesn't gets used in lower versions
// https://docs.microsoft.com/en-us/windows/console/getconsolemode
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace rang {

/* For better compability with most of terminals do not use any style settings
 * except of reset, bold and reversed.
 * Note that on Windows terminals bold style is same as fgB color.
 */
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

/*
 * Behaviour of rang function calls
 *  - Off   - toggle off rang style/color calls
 *  - Auto  - (Default) autodect terminal and colorize if needed
 *  - Force - force ansi color output to non terminal streams
 */
enum class control { Off = 0, Auto = 1, Force = 2 };

/*
 * Use rang::setWinTermMode to explicitly set terminal API for Windows
 * Calling rang::setWinTermMode have no effect on other OS
 *  - Auto  - (Default) automatically detects wheter Ansi or Native API
 *  - Ansi   - Force use Ansi API
 *  - Native - Force use Native API
 */
enum class winTerm { Auto = 0, Ansi = 1, Native = 2 };


namespace rang_implementation {

    inline rang::control &controlMode() noexcept
    {
        static rang::control value{ rang::control::Auto };
        return value;
    }

    inline rang::winTerm &winTermMode() noexcept
    {
        static rang::winTerm termMode{ rang::winTerm::Auto };
        return termMode;
    }

    template <typename _CharT, typename _Traits>
    inline FILE *
    stdio_file(const std::basic_streambuf<_CharT, _Traits> *sbuf) noexcept
    {
        return nullptr;
    }

    inline FILE *stdio_file(const std::wstreambuf *sbuf) noexcept
    {
        if (sbuf == std::wcout.rdbuf()) {
            return stdout;
        } else if (sbuf == std::wcerr.rdbuf() || sbuf == std::wclog.rdbuf()) {
            return stderr;
        }
        return nullptr;
    }

    inline FILE *stdio_file(const std::streambuf *sbuf) noexcept
    {
        if (sbuf == std::cout.rdbuf()) {
            return stdout;
        } else if (sbuf == std::cerr.rdbuf() || sbuf == std::clog.rdbuf()) {
            return stderr;
        }
        return nullptr;
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
            }
            return std::any_of(std::begin(Terms), std::end(Terms),
                               [&](const char *term) {
                                   return std::strstr(env_p, term) != nullptr;
                               });
        }();

#elif defined(OS_WIN)
        // All windows versions support colors through native console methods
        static constexpr bool result = true;
#endif
        return result;
    }

#ifdef OS_WIN


    inline bool isMsysPty(int fd) noexcept
    {
        // Dynamic load for binary compability with old Windows
        const auto ptrGetFileInformationByHandleEx
          = reinterpret_cast<decltype(&GetFileInformationByHandleEx)>(
            GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
                           "GetFileInformationByHandleEx"));
        if (!ptrGetFileInformationByHandleEx) {
            return false;
        }

        HANDLE h = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
        if (h == INVALID_HANDLE_VALUE) {
            return false;
        }

        // Check that it's a pipe:
        if (GetFileType(h) != FILE_TYPE_PIPE) {
            return false;
        }

        // POD type is binary compatible with FILE_NAME_INFO from WinBase.h
        // It have the same alignment and used to avoid UB in caller code
        struct MY_FILE_NAME_INFO {
            DWORD FileNameLength;
            WCHAR FileName[MAX_PATH];
        };

        auto pNameInfo = std::unique_ptr<MY_FILE_NAME_INFO>(
          new (std::nothrow) MY_FILE_NAME_INFO());
        if (!pNameInfo) {
            return false;
        }

        // Check pipe name is template of
        // {"cygwin-","msys-"}XXXXXXXXXXXXXXX-ptyX-XX
        if (!ptrGetFileInformationByHandleEx(h, FileNameInfo, pNameInfo.get(),
                                             sizeof(MY_FILE_NAME_INFO))) {
            return false;
        }
        std::wstring name(pNameInfo->FileName,
                          pNameInfo->FileNameLength / sizeof(WCHAR));
        if ((name.find(L"msys-") == std::wstring::npos
             && name.find(L"cygwin-") == std::wstring::npos)
            || name.find(L"-pty") == std::wstring::npos) {
            return false;
        }

        return true;
    }

#endif

    template <typename _CharT, typename _Traits>
    inline bool
    isTerminal(std::basic_streambuf<_CharT, _Traits> *osbuf) noexcept
    {
        const FILE *ioFile = stdio_file(osbuf);
#if defined(OS_LINUX) || defined(OS_MAC)
        if (ioFile == stdout) {
            static const bool cout_term = isatty(fileno(stdout)) != 0;
            return cout_term;
        } else if (ioFile == stderr) {
            static const bool cerr_term = isatty(fileno(stderr)) != 0;
            return cerr_term;
        }
#elif defined(OS_WIN)
        if (ioFile == stdout) {
            static const bool cout_term
              = (_isatty(_fileno(stdout)) || isMsysPty(_fileno(stdout)));
            return cout_term;
        } else if (ioFile == stderr) {
            static const bool cerr_term
              = (_isatty(_fileno(stderr)) || isMsysPty(_fileno(stderr)));
            return cerr_term;
        }
#endif
        return false;
    }

    template <typename T>
    using enableRang = typename std::enable_if<
      std::is_same<T, rang::style>::value || std::is_same<T, rang::fg>::value
        || std::is_same<T, rang::bg>::value || std::is_same<T, rang::fgB>::value
        || std::is_same<T, rang::bgB>::value,
      T>::type;

#ifdef OS_WIN

    struct SGR {  // Select Graphic Rendition parameters for Windows console
        BYTE fgColor;  // foreground color (0-15) lower 3 rgb bits + intense bit
        BYTE bgColor;  // background color (0-15) lower 3 rgb bits + intense bit
        BYTE bold;  // emulated as FOREGROUND_INTENSITY bit
        BYTE underline;  // emulated as BACKGROUND_INTENSITY bit
        BOOLEAN inverse;  // swap foreground/bold & background/underline
        BOOLEAN conceal;  // set foreground/bold to background/underline
    };

    enum class AttrColor : BYTE {  // Color attributes for console screen buffer
        black   = 0,
        red     = 4,
        green   = 2,
        yellow  = 6,
        blue    = 1,
        magenta = 5,
        cyan    = 3,
        gray    = 7
    };

    template <typename _CharT, typename _Traits>
    inline HANDLE getConsoleHandle(
      const std::basic_streambuf<_CharT, _Traits> *osbuf) noexcept
    {
        const FILE *ioFile = stdio_file(osbuf);
        if (ioFile == stdout) {
            static const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
            return hStdout;
        } else if (ioFile == stderr) {
            static const HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
            return hStderr;
        }
        return INVALID_HANDLE_VALUE;
    }

    template <typename _CharT, typename _Traits>
    inline bool setWinTermAnsiColors(
      const std::basic_streambuf<_CharT, _Traits> *osbuf) noexcept
    {
        HANDLE h = getConsoleHandle(osbuf);
        if (h == INVALID_HANDLE_VALUE) {
            return false;
        }
        DWORD dwMode = 0;
        if (!GetConsoleMode(h, &dwMode)) {
            return false;
        }
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(h, dwMode)) {
            return false;
        }
        return true;
    }

    template <typename _CharT, typename _Traits>
    inline bool
    supportsAnsi(const std::basic_streambuf<_CharT, _Traits> *osbuf) noexcept
    {
        FILE *ioFile = stdio_file(osbuf);
        if (ioFile == stdout) {
            static const bool cout_ansi
              = (isMsysPty(_fileno(stdout)) || setWinTermAnsiColors(osbuf));
            return cout_ansi;
        } else if (ioFile == stderr) {
            static const bool cerr_ansi
              = (isMsysPty(_fileno(stderr)) || setWinTermAnsiColors(osbuf));
            return cerr_ansi;
        }
        return false;
    }

    inline const SGR &defaultState() noexcept
    {
        static const SGR defaultSgr = []() -> SGR {
            CONSOLE_SCREEN_BUFFER_INFO info;
            WORD attrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                                           &info)
                || GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),
                                              &info)) {
                attrib = info.wAttributes;
            }
            SGR sgr     = { 0, 0, 0, 0, FALSE, FALSE };
            sgr.fgColor = attrib & 0x0F;
            sgr.bgColor = (attrib & 0xF0) >> 4;
            return sgr;
        }();
        return defaultSgr;
    }

    inline BYTE ansi2attr(BYTE rgb) noexcept
    {
        static const AttrColor rev[8]
          = { AttrColor::black,  AttrColor::red,  AttrColor::green,
              AttrColor::yellow, AttrColor::blue, AttrColor::magenta,
              AttrColor::cyan,   AttrColor::gray };
        return static_cast<BYTE>(rev[rgb]);
    }

    inline void setWinSGR(rang::bg col, SGR &state) noexcept
    {
        if (col != rang::bg::reset) {
            state.bgColor = ansi2attr(static_cast<BYTE>(col) - 40);
        } else {
            state.bgColor = defaultState().bgColor;
        }
    }

    inline void setWinSGR(rang::fg col, SGR &state) noexcept
    {
        if (col != rang::fg::reset) {
            state.fgColor = ansi2attr(static_cast<BYTE>(col) - 30);
        } else {
            state.fgColor = defaultState().fgColor;
        }
    }

    inline void setWinSGR(rang::bgB col, SGR &state) noexcept
    {
        state.bgColor = (BACKGROUND_INTENSITY >> 4)
          | ansi2attr(static_cast<BYTE>(col) - 100);
    }

    inline void setWinSGR(rang::fgB col, SGR &state) noexcept
    {
        state.fgColor
          = FOREGROUND_INTENSITY | ansi2attr(static_cast<BYTE>(col) - 90);
    }

    inline void setWinSGR(rang::style style, SGR &state) noexcept
    {
        switch (style) {
            case rang::style::reset: state = defaultState(); break;
            case rang::style::bold: state.bold = FOREGROUND_INTENSITY; break;
            case rang::style::underline:
            case rang::style::blink:
                state.underline = BACKGROUND_INTENSITY;
                break;
            case rang::style::reversed: state.inverse = TRUE; break;
            case rang::style::conceal: state.conceal = TRUE; break;
            default: break;
        }
    }

    inline SGR &current_state() noexcept
    {
        static SGR state = defaultState();
        return state;
    }

    inline WORD SGR2Attr(const SGR &state) noexcept
    {
        WORD attrib = 0;
        if (state.conceal) {
            if (state.inverse) {
                attrib = (state.fgColor << 4) | state.fgColor;
                if (state.bold)
                    attrib |= FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
            } else {
                attrib = (state.bgColor << 4) | state.bgColor;
                if (state.underline)
                    attrib |= FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
            }
        } else if (state.inverse) {
            attrib = (state.fgColor << 4) | state.bgColor;
            if (state.bold) attrib |= BACKGROUND_INTENSITY;
            if (state.underline) attrib |= FOREGROUND_INTENSITY;
        } else {
            attrib = state.fgColor | (state.bgColor << 4) | state.bold
              | state.underline;
        }
        return attrib;
    }

    template <typename _CharT, typename _Traits, typename T,
              typename = enableRang<T>>
    inline void setWinColorAnsi(std::basic_ostream<_CharT, _Traits> &os,
                                T const value)
    {
        os << "\033[" << static_cast<int>(value) << "m";
    }

    template <typename _CharT, typename _Traits, typename T,
              typename = enableRang<T>>
    inline void setWinColorNative(std::basic_ostream<_CharT, _Traits> &os,
                                  T const value)
    {
        const HANDLE h = getConsoleHandle(os.rdbuf());
        if (h != INVALID_HANDLE_VALUE) {
            setWinSGR(value, current_state());
            os.flush();  // necessary
            SetConsoleTextAttribute(h, SGR2Attr(current_state()));
        }
    }

    template <typename _CharT, typename _Traits, typename T,
              typename = enableRang<T>>
    inline std::basic_ostream<_CharT, _Traits> &
    setColor(std::basic_ostream<_CharT, _Traits> &os, T const value)
    {
        if (winTermMode() == winTerm::Auto) {
            if (supportsAnsi(os.rdbuf())) {
                setWinColorAnsi(os, value);
            } else {
                setWinColorNative(os, value);
            }
        } else if (winTermMode() == winTerm::Ansi) {
            setWinColorAnsi(os, value);
        } else {
            setWinColorNative(os, value);
        }
        return os;
    }
#else
    template <typename _CharT, typename _Traits, typename T,
              typename = enableRang<T>>
    inline std::basic_ostream<_CharT, _Traits> &
    setColor(std::basic_ostream<_CharT, _Traits> &os, T const value)
    {
        return os << "\033[" << static_cast<int>(value) << "m";
    }
#endif
}  // namespace rang_implementation

template <typename _CharT, typename _Traits, typename T,
          typename = rang_implementation::enableRang<T>>
inline std::basic_ostream<_CharT, _Traits> &
operator<<(std::basic_ostream<_CharT, _Traits> &os, const T &value)
{
    const control option = rang_implementation::controlMode();
    switch (option) {
        case control::Auto:
            return rang_implementation::supportsColor()
                && rang_implementation::isTerminal(os.rdbuf())
              ? rang_implementation::setColor(os, value)
              : os;
        case control::Force: return rang_implementation::setColor(os, value);
        default: return os;
    }
}

inline void setWinTermMode(const rang::winTerm value) noexcept
{
    rang_implementation::winTermMode() = value;
}

inline void setControlMode(const control value) noexcept
{
    rang_implementation::controlMode() = value;
}

}  // namespace rang

#undef OS_LINUX
#undef OS_WIN
#undef OS_MAC

#endif /* ifndef RANG_DOT_HPP */
