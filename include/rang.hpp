#ifndef RANG_DOT_HPP
#define RANG_DOT_HPP

#define RANG_VERSION_MAJOR 4
#define RANG_VERSION_MINOR 0
#define RANG_VERSION_PATCH 0

#define RANG_VER_CHECK(major, minor, patch)                                    \
    (((major) << 16) | ((minor) << 8) | (patch))
#define RANG_VER                                                               \
    RANG_VER_CHECK(RANG_VERSION_MAJOR, RANG_VERSION_MINOR, RANG_VERSION_PATCH)

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
#include <atomic>
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
 * Use rang::setControlMode() to set rang control mode
 *  Off  : Toggle off rang style/color calls
 *  Auto : Autodetect terminal and colorize if needed (Default)
 *  Force: Force ansi color output to non terminal streams
 */
enum class control { Off = 0, Auto = 1, Force = 2 };

/*
 * Use rang::setWinTermMode() to explicitly set terminal API for Windows
 * Calling rang::setWinTermMode() have no effect on other OS
 *  Auto  : Automatically detects whether Ansi or Native API (Default)
 *  Ansi  : Force use Ansi API
 *  Native: Force use Native API
 */
enum class winTerm { Auto = 0, Ansi = 1, Native = 2 };

// Do not open, part of implementation
namespace rang_implementation {

    inline std::atomic<control> &controlMode() noexcept
    {
        static std::atomic<control> value(control::Auto);
        return value;
    }

    inline std::atomic<winTerm> &winTermMode() noexcept
    {
        static std::atomic<winTerm> termMode(winTerm::Auto);
        return termMode;
    }

    template <typename CharT, typename Traits>
    inline FILE *
    stdio_file(const std::basic_streambuf<CharT, Traits> *sbuf) noexcept
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

    inline bool isSupportedTerm() noexcept
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

    template <typename CharT, typename Traits>
    inline bool isTTY(std::basic_streambuf<CharT, Traits> *osbuf) noexcept
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

    template <typename CharT, typename Traits>
    class StreamScopeGuard {
        std::basic_ostream<CharT, Traits> &os;
        const std::ios_base::fmtflags flags;
        const std::streamsize width;
        const std::streamsize precision;

    public:
        StreamScopeGuard(std::basic_ostream<CharT, Traits> &_os)
            : os(_os),
              flags(os.flags()),
              width(os.width()),
              precision(os.precision())
        {
            os.width(0);
            os.precision(0);
        }

        ~StreamScopeGuard()
        {
            os.flags(flags);
            os.width(width);
            os.precision(precision);
        }
    };

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

    template <typename CharT, typename Traits>
    inline HANDLE
    getConsoleHandle(const std::basic_streambuf<CharT, Traits> *osbuf) noexcept
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

    template <typename CharT, typename Traits>
    inline bool setWinTermAnsiColors(
      const std::basic_streambuf<CharT, Traits> *osbuf) noexcept
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

    template <typename CharT, typename Traits>
    inline bool
    supportsAnsi(const std::basic_streambuf<CharT, Traits> *osbuf) noexcept
    {
        const FILE *ioFile = stdio_file(osbuf);
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

    template <typename CharT, typename Traits, typename T,
              typename = enableRang<T>>
    inline void setWinColorAnsi(std::basic_ostream<CharT, Traits> &os,
                                T const value)
    {
        StreamScopeGuard<CharT, Traits> guard(os);
        os.flags(std::ios::dec | std::ios::left);
        os << "\033[" << static_cast<int>(value) << "m";
    }

    template <typename CharT, typename Traits, typename T,
              typename = enableRang<T>>
    inline void setWinColorNative(std::basic_ostream<CharT, Traits> &os,
                                  T const value)
    {
        const HANDLE h = getConsoleHandle(os.rdbuf());
        if (h != INVALID_HANDLE_VALUE) {
            setWinSGR(value, current_state());
            os.flush();  // necessary
            SetConsoleTextAttribute(h, SGR2Attr(current_state()));
        }
    }

    template <typename CharT, typename Traits, typename T,
              typename = enableRang<T>>
    inline std::basic_ostream<CharT, Traits> &
    setColor(std::basic_ostream<CharT, Traits> &os, T const value)
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
    template <typename CharT, typename Traits, typename T,
              typename = enableRang<T>>
    inline std::basic_ostream<CharT, Traits> &
    setColor(std::basic_ostream<CharT, Traits> &os, T const value)
    {
        StreamScopeGuard<CharT, Traits> guard(os);
        os.flags(std::ios::dec | std::ios::left);
        return os << "\033[" << static_cast<int>(value) << "m";
    }
#endif

#if defined(OS_WIN)
    template <typename CharT, typename Traits>
    inline void showCursorNative(const std::basic_ostream<CharT, Traits> &os,
                                 const bool flg) noexcept
    {
        const HANDLE h = getConsoleHandle(os.rdbuf());
        if (h == INVALID_HANDLE_VALUE) return;

        CONSOLE_CURSOR_INFO info;
        if (!GetConsoleCursorInfo(h, &info)) return;

        info.bVisible = flg ? TRUE : FALSE;
        SetConsoleCursorInfo(h, &info);
    }
#endif

    // CRTP base class
    template <typename T>
    class Cursor {
        Cursor() = default;
        friend T;
    };
}  // namespace rang_implementation

inline void setWinTermMode(const rang::winTerm value) noexcept
{
    rang_implementation::winTermMode() = value;
}

inline void setControlMode(const control value) noexcept
{
    rang_implementation::controlMode() = value;
}

namespace cursor {

    struct setVisible final : public rang_implementation::Cursor<setVisible> {
        const bool visible;
        setVisible(const bool v = true) : visible(v) {}

        template <typename CharT, typename Traits>
        void execAnsi(std::basic_ostream<CharT, Traits> &os) const
        {
            os << (visible ? "\033[?25h" : "\033[?25l");
        }
#if defined(OS_WIN)
        template <typename CharT, typename Traits>
        void execNative(const std::basic_ostream<CharT, Traits> &os) const
          noexcept
        {
            rang_implementation::showCursorNative(os, visible);
        }
#endif
    };

    struct setPos final : public rang_implementation::Cursor<setPos> {
        const short x;
        const short y;
        setPos(const short a = 0, const short b = 0) : x(a), y(b) {}

        template <typename CharT, typename Traits>
        void execAnsi(std::basic_ostream<CharT, Traits> &os) const
        {
            os << "\033[" << x << ';' << y << 'H';
        }
#if defined(OS_WIN)
        template <typename CharT, typename Traits>
        void execNative(const std::basic_ostream<CharT, Traits> &os) const
          noexcept
        {
            using namespace rang_implementation;
            const auto consoleHandle = getConsoleHandle(os.rdbuf());
            if (consoleHandle != INVALID_HANDLE_VALUE) {
                SetConsoleCursorPosition(consoleHandle, { y, x });
            }
        }
#endif
    };
}  // namespace cursor

}  // namespace rang


// Applies cursor operation to given ostream
template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os,
           const rang::rang_implementation::Cursor<T> &&base)
{
    using namespace rang;
    using namespace rang_implementation;

    const auto useCursor = [&]() -> std::basic_ostream<CharT, Traits> & {
        const auto &&drv = static_cast<T const &&>(base);
#if defined(OS_LINUX) || defined(OS_MAC)
        StreamScopeGuard<CharT, Traits> guard(os);
        os.flags(std::ios::dec | std::ios::left);
        drv.execAnsi(os);
        os.flush();
#elif defined(OS_WIN)
        if (winTermMode() == winTerm::Auto) {
            if (supportsAnsi(os.rdbuf())) {
                StreamScopeGuard<CharT, Traits> guard(os);
                os.flags(std::ios::dec | std::ios::left);
                drv.execAnsi(os);
                os.flush();
            } else {
                drv.execNative(os);
            }
        } else if (winTermMode() == winTerm::Ansi) {
            StreamScopeGuard<CharT, Traits> guard(os);
            os.flags(std::ios::dec | std::ios::left);
            drv.execAnsi(os);
            os.flush();
        } else {
            drv.execNative(os);
        }
#endif
        return os;
    };

    const control option = controlMode();
    switch (option) {
        case control::Auto:
            return isSupportedTerm() && isTTY(os.rdbuf()) ? useCursor() : os;
        case control::Force: return useCursor();
        default: return os;
    }
}

// Applies color operation to given ostream
template <typename CharT, typename Traits, typename T,
          typename = rang::rang_implementation::enableRang<T>>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const T &value)
{
    using namespace rang;
    using namespace rang_implementation;

    const control option = controlMode();
    switch (option) {
        case control::Auto:
            return isSupportedTerm() && isTTY(os.rdbuf()) ? setColor(os, value)
                                                          : os;
        case control::Force: return setColor(os, value);
        default: return os;
    }
}

#undef OS_LINUX
#undef OS_WIN
#undef OS_MAC

#endif /* ifndef RANG_DOT_HPP */
