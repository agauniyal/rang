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
#include <termios.h>
#elif defined(OS_WIN)
#include <windows.h>
#include <io.h>
#include <VersionHelpers.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <atomic>
#include <mutex>
#include <map>

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

enum class control {
    offColor,
    autoColor,
    forceColor,
    blocking,
    nonBlocking,
    flush,
    lowLatency,
    normalLatency
};

namespace rang_implementation {

    // Aliased as rang::Key
    class Key {
    public:
        enum Which : uint32_t {
            Empty = 0x80000000,
            Escape, Up, Down, Right, Left,
            Insert, Delete, PageUp, PageDown, Home, End,
            F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
            Backspace
        };

        Key(uint32_t w = static_cast<uint32_t>(Which::Empty)) : which(w) { }
        Key(Which w) : which(static_cast<uint32_t>(w)) { }

        bool isCharacter() { return which < 0x80000000; }
        bool isAscii() { return which < 0x0000007F; }
        bool isPrintable() { return which > ' ' && isCharacter(); }
        bool isPrintableAscii() { return which > ' ' && isAscii(); }
        bool isSpecial() { return which > 0x80000000; }
        bool isEmpty() { return which == 0x80000000; }

        operator int() const { return static_cast<int>(which); }

        friend std::istream& operator>>(std::istream&, Key&);
    private:
        uint32_t which;
    };
    
	inline std::streambuf const *&RANG_coutbuf()
	{
		static std::streambuf const *pOutbuff = std::cout.rdbuf();
		return pOutbuff;
	}

	inline std::streambuf const *&RANG_cerrbuf()
	{
		static std::streambuf const *pErrbuff = std::cerr.rdbuf();
		return pErrbuff;
	}

	inline std::streambuf const *&RANG_clogbuf()
	{
		static std::streambuf const *pLogbuff = std::clog.rdbuf();
		return pLogbuff;
	}

	inline std::atomic<bool>& isColorForced()
	{
        static std::atomic<bool> flag(false);
		return flag;
	}

	inline std::atomic<bool>& isColorOff()
	{
        static std::atomic<bool> flag(false);
		return flag;
	}

	inline std::atomic<bool>& isColorSupported()
	{
        static std::atomic<bool> flag(false);
		return flag;
	}

    inline std::atomic<bool>& isInputBlocking()
    {
        static std::atomic<bool> flag(true);
		return flag;
    }

    inline std::atomic<bool>& isInputLowLatency()
    {
        static std::atomic<bool> flag(false);
		return flag;
    }

    inline std::mutex& getTerminalControlMutex() {
        static std::mutex m;
        return m;
    }
    

	inline bool supportsColor()
	{
#if defined(OS_LINUX) || defined(OS_MAC)
		static constexpr const char *Terms[] = {
			"ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm",
			"linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm",
            "Eterm"
		};

		static const char *env_p = std::getenv("TERM");

		static const bool result = (env_p == nullptr)
		  ? false
		  : std::any_of(
		      std::begin(Terms), std::end(Terms), [](const char *term) {
			      return std::strstr(env_p, term) != nullptr;
			  });

#elif defined(OS_WIN)
		static constexpr bool result = true;
#endif
		return result;
	}


	inline bool isTerminal(const std::streambuf *osbuf)
	{
		if (osbuf == RANG_coutbuf()) {
#if defined(OS_LINUX) || defined(OS_MAC)
			return isatty(fileno(stdout)) ? true : false;
#elif defined(OS_WIN)
			return _isatty(_fileno(stdout)) ? true : false;
#endif
		}

		if (osbuf == RANG_cerrbuf() || osbuf == RANG_clogbuf()) {
#if defined(OS_LINUX) || defined(OS_MAC)
			return isatty(fileno(stderr)) ? true : false;
#elif defined(OS_WIN)
			return _isatty(_fileno(stderr)) ? true : false;
#endif
		}
		return false;
	}


	template <typename T>
	using enableStd =
	  typename std::enable_if<std::is_same<T, rang::style>::value
	      || std::is_same<T, rang::fg>::value
	      || std::is_same<T, rang::bg>::value
	      || std::is_same<T, rang::fgB>::value
	      || std::is_same<T, rang::bgB>::value,
	    std::ostream &>::type;


#ifdef OS_WIN
	inline HANDLE getVersionDependentHandle(DWORD which)
	{
		if (IsWindowsVersionOrGreater(10, 0, 0)) return nullptr;
		return GetStdHandle(which);
	}

	inline HANDLE getVersionIndependentHandle(DWORD which)
	{
		return GetStdHandle(which);
	}

	inline HANDLE getConsoleHandle()
	{
		static HANDLE h = getVersionDependentHandle(STD_OUTPUT_HANDLE);
		return h;
	}

	inline HANDLE getInputHandle()
	{
		static HANDLE h = getVersionIndependentHandle(STD_INPUT_HANDLE);
		return h;
	}

	inline WORD reverseRGB(WORD rgb)
	{
		static const WORD rev[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
		return rev[rgb];
	}

	inline void setWinAttribute(rang::bg col, WORD &state)
	{
		state &= 0xFF0F;
		state |= reverseRGB(static_cast<WORD>(col) - 40) << 4;
	}

	inline void setWinAttribute(rang::fg col, WORD &state)
	{
		state &= 0xFFF0;
		state |= reverseRGB(static_cast<WORD>(col) - 30);
	}

	inline void setWinAttribute(rang::bgB col, WORD &state)
	{
		state &= 0xFF0F;
		state |= (0x8 | reverseRGB(static_cast<WORD>(col) - 100)) << 4;
	}

	inline void setWinAttribute(rang::fgB col, WORD &state)
	{
		state &= 0xFFF0;
		state |= (0x8 | reverseRGB(static_cast<WORD>(col) - 90));
	}

	inline void setWinAttribute(rang::style style, WORD &state)
	{
		if (style == rang::style::reset) {
			state = (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		}
	}

	inline WORD &current_state()
	{
		static WORD state
		  = (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		return state;
	}


	template <typename T>
	inline enableStd<T> setColor(std::ostream &os, T const value)
	{
		HANDLE h = getConsoleHandle();
		if (h && isTerminal(os.rdbuf())) {
			setWinAttribute(value, current_state());
			SetConsoleTextAttribute(h, current_state());
			return os;
		}
		return os << "\033[" << static_cast<int>(value) << "m";
	}

    uint32_t readUnbuffered() {
        std::lock_guard<std::mutex> guard(getTerminalControlMutex());
        static std::map<WORD, Key::Which> keyMap = {
            { VK_ESCAPE, Key::Which::Escape },
            { VK_UP, Key::Which::Up },
            { VK_DOWN, Key::Which::Down },
            { VK_RIGHT, Key::Which::Right },
            { VK_LEFT, Key::Which::Left },
            { VK_INSERT, Key::Which::Insert },
            { VK_DELETE, Key::Which::Delete },
            { VK_PRIOR, Key::Which::PageUp },
            { VK_NEXT, Key::Which::PageDown },
            { VK_HOME, Key::Which::Home },
            { VK_END, Key::Which::End },
            { VK_F1, Key::Which::F1 },
            { VK_F2, Key::Which::F2 },
            { VK_F3, Key::Which::F3 },
            { VK_F4, Key::Which::F4 },
            { VK_F5, Key::Which::F5 },
            { VK_F6, Key::Which::F6 },
            { VK_F7, Key::Which::F7 },
            { VK_F8, Key::Which::F8 },
            { VK_F9, Key::Which::F9 },
            { VK_F10, Key::Which::F10 },
            { VK_F11, Key::Which::F11 },
            { VK_F12, Key::Which::F12 },
            { VK_BACK, Key::Which::Backspace }
        };

        INPUT_RECORD ir;
        DWORD nEvents;
        auto h = getInputHandle();

        // Itertae to skip over untranslated events
        while(true) {
            // See if an input event is waiting
            if(PeekConsoleInput(h, &ir, 1, &nEvents) == 0 ||
                nEvents == 0) {
                if(isInputBlocking()) {
                    continue;
                }
                return static_cast<uint32_t>(Key::Which::Empty);
            }

            // Try to read the input event
            if(ReadConsoleInput(h, &ir, 1, &nEvents) == 0 ||
                nEvents == 0) {
                if(isInputBlocking()) {
                    continue;
                }
                return static_cast<uint32_t>(Key::Which::Empty);
            }

            // Translate the input event into a keycode
            if(ir.EventType == KEY_EVENT) {
                // Make sure this is a keypress
                if(ir.Event.KeyEvent.bKeyDown == FALSE) {
                    if(isInputBlocking()) {
                        continue;
                    }
                    return static_cast<uint32_t>(Key::Which::Empty);
                }

                // Non-printable keys
                auto it = keyMap.find(ir.Event.KeyEvent.wVirtualKeyCode);
                if(it != keyMap.end()) {
                    return static_cast<uint32_t>(it->second);
                }

                // Untranslated non-printable key
                if(ir.Event.KeyEvent.uChar.UnicodeChar == '\0') {
                    if(isInputBlocking()) {
                        continue;
                    }
                    return static_cast<uint32_t>(Key::Which::Empty);
                }

                return static_cast<uint32_t>(
                    ir.Event.KeyEvent.uChar.UnicodeChar);
            }
        }
    }

    void flushInput() {
        std::lock_guard<std::mutex> guard(getTerminalControlMutex());

        INPUT_RECORD ir;
        DWORD nEvents;
        auto h = getInputHandle();

        while(PeekConsoleInput(h, &ir, 1, &nEvents) == 0 &&
            nEvents != 0) {
            ReadConsoleInput(h, &ir, 1, &nEvents);
        }
    }
#else
	template <typename T>
	inline enableStd<T> setColor(std::ostream &os, T const value)
	{
		return os << "\033[" << static_cast<int>(value) << "m";
	}

    class UnbufferedTerminalInputContext {
    public:
        UnbufferedTerminalInputContext() {
            bufferInput(false);
        }
        ~UnbufferedTerminalInputContext() {
            bufferInput(true);
        }
        void setWait(bool wait) {
            bufferInput(false, wait);
        }
    private:
        void bufferInput(bool buffer, bool forceWait = false) {
            if(!isatty(fileno(stdin))) {
                return;
            }
            termios attr;

            tcgetattr(fileno(stdin), &attr);
            if(buffer) {
                attr.c_lflag |= ICANON;
                // This attribute may be overloaded when ICANON is in effect,
                // so set it back to default 0.
                attr.c_cc[VTIME] = 0;
            } else {
                attr.c_lflag &= ~ICANON;
                if(forceWait == true ||
                    isInputLowLatency() == false) {
                    // Wait one decasecond before returning EOF when no data is
                    // pending. This prevents high CPU utilization
                    // while waiting for input.
                    attr.c_cc[VTIME] = 1;
                } else {
                    attr.c_cc[VTIME] = 0;
                }
            }
            attr.c_cc[VMIN] = 0;

            tcsetattr(fileno(stdin), TCSANOW, &attr);
        }
    };
    
    uint32_t readUnbuffered() {
        enum State { Normal, Escape, ANSI, Xterm, XtermTeens, XtermTwenties,
            HiXtermDone };
        State state = Normal;
        uint32_t hiXtermKey = static_cast<uint32_t>(Key::Which::Empty);
        std::lock_guard<std::mutex> guard(getTerminalControlMutex());
        UnbufferedTerminalInputContext utic;

        // Iterate over non-translated characters
        while(true) {
            int c = fgetc(stdin);
            switch(state) {
            case Normal:
                switch(c) {
                case '\033':
                    state = Escape;
                    utic.setWait(true);
                    break;
                case '\177':
                    return static_cast<uint32_t>(Key::Which::Backspace);
                case EOF:
                    if(isInputBlocking() == false) {
                        return static_cast<uint32_t>(Key::Which::Empty);
                    }
                    break;
                default:
                    return static_cast<uint32_t>(c);
                }
                break;
            case Escape:
                switch(c) {
                case EOF:
                    // Bare escape
                    return static_cast<uint32_t>(Key::Which::Escape);
                case 'O':
                    state = ANSI;
                    break;
                case '[':
                    state = Xterm;
                    break;
                default:
                    // Unknown escape sequence, \033 eaten
                    return static_cast<uint32_t>(c);
                }
                break;
            case ANSI:
                switch(c) {
                case 'H':
                    return static_cast<uint32_t>(Key::Which::Home);
                case 'F':
                    return static_cast<uint32_t>(Key::Which::End);
                case 'P':
                    return static_cast<uint32_t>(Key::Which::F1);
                case 'Q':
                    return static_cast<uint32_t>(Key::Which::F2);
                case 'R':
                    return static_cast<uint32_t>(Key::Which::F3);
                case 'S':
                    return static_cast<uint32_t>(Key::Which::F4);
                default:
                    // Unknown escape sequence, \033 O eaten
                    return static_cast<uint32_t>(c);
                }
                break;
            case Xterm:
                switch(c) {
                case 'A':
                    return static_cast<uint32_t>(Key::Which::Up);
                case 'B':
                    return static_cast<uint32_t>(Key::Which::Down);
                case 'C':
                    return static_cast<uint32_t>(Key::Which::Right);
                case 'D':
                    return static_cast<uint32_t>(Key::Which::Left);
                case 'H':
                    return static_cast<uint32_t>(Key::Which::Home);
                case 'F':
                    return static_cast<uint32_t>(Key::Which::End);
                case '1':
                    state = XtermTeens;
                    break;
                case '2':
                    state = XtermTwenties;
                    break;
                case '3':
                    return static_cast<uint32_t>(Key::Which::Delete);
                case '4':
                    return static_cast<uint32_t>(Key::Which::End);
                case '5':
                    return static_cast<uint32_t>(Key::Which::PageUp);
                case '6':
                    return static_cast<uint32_t>(Key::Which::PageDown);
                case '7':
                    return static_cast<uint32_t>(Key::Which::Home);
                case '8':
                    return static_cast<uint32_t>(Key::Which::End);
                default:
                    // Unknown escape sequence, \033 [ eaten
                    return static_cast<uint32_t>(c);
                }
                break;
            case XtermTeens:
                switch(c) {
                case '~':
                    // \033 [ 1 ~
                    return static_cast<uint32_t>(Key::Which::Home);
                case '1':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F1);
                    state = HiXtermDone;
                    break;
                case '2':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F2);
                    state = HiXtermDone;
                    break;
                case '3':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F3);
                    state = HiXtermDone;
                    break;
                case '4':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F4);
                    state = HiXtermDone;
                    break;
                case '5':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F5);
                    state = HiXtermDone;
                    break;
                case '7':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F6);
                    state = HiXtermDone;
                    break;
                case '8':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F7);
                    state = HiXtermDone;
                    break;
                case '9':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F8);
                    state = HiXtermDone;
                    break;
                default:
                    // Unknown escape sequence, \033 [ 1 eaten
                    return static_cast<uint32_t>(c);
                }
                break;
            case XtermTwenties:
                switch(c) {
                case '~':
                    // \033 [ 2 ~
                    return static_cast<uint32_t>(Key::Which::Insert);
                case '0':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F9);
                    state = HiXtermDone;
                    break;
                case '1':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F10);
                    state = HiXtermDone;
                    break;
                case '3':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F11);
                    state = HiXtermDone;
                    break;
                case '4':
                    hiXtermKey = static_cast<uint32_t>(Key::Which::F12);
                    state = HiXtermDone;
                    break;
                default:
                    // Unknown escape sequence, \033 [ 2 eaten
                    return static_cast<uint32_t>(c);
                }
                break;
            case HiXtermDone:
                if(c == '~') {
                    return hiXtermKey;
                } 
                // Unknown escape sequence, \033 [ ? ? eaten
                return static_cast<uint32_t>(c);
            }
        }
    }

    void flushInput() {
        std::lock_guard<std::mutex> guard(getTerminalControlMutex());

        UnbufferedTerminalInputContext utic;

        while(fgetc(stdin) != EOF) {
            // Ignore
        }
    }
#endif

	template <typename T, typename S>
	using enableControl = typename std::enable_if<
        std::is_same<T, rang::control>::value, S
    >::type;

    std::istream& operator >> (std::istream& input, Key& r) {
        r.which = readUnbuffered();
        return input;
    }
}

using Key = rang_implementation::Key;

void init()
{
	rang_implementation::RANG_coutbuf();
	rang_implementation::RANG_cerrbuf();
	rang_implementation::RANG_clogbuf();
    rang_implementation::isInputBlocking() = true;
    rang_implementation::isInputLowLatency() = false;
}


template <typename T>
inline rang_implementation::enableStd<T> operator<<(
  std::ostream &os, T const value)
{
	std::streambuf const *osbuf = os.rdbuf();
    if(rang_implementation::isColorOff()) {
        return os;
    } else if(rang_implementation::isColorForced()) {
        return rang_implementation::setColor(os, value);
    } else if(rang_implementation::supportsColor() &&
        rang_implementation::isTerminal(osbuf)) {
        return rang_implementation::setColor(os, value);
    } else {
        return os;
    }
}


template <typename T>
inline rang_implementation::enableControl<T, std::ostream&> operator<<(
  std::ostream &os, T const value)
{
    switch(value) {
        case rang::control::forceColor:
            rang_implementation::isColorForced() = true;
            break;
        case rang::control::autoColor:
            rang_implementation::isColorForced() = false;
            rang_implementation::isColorOff() = false;
            break;
        case rang::control::offColor:
            rang_implementation::isColorOff() = true;
            break;
        default:
            // Ignore
            break;
    }
	return os;
}

template <typename T>
inline rang_implementation::enableControl<T, std::istream&> operator>>(
  std::istream &is, T const value)
{
    switch(value) {
        case rang::control::blocking:
            rang_implementation::isInputBlocking() = true;
            break;
        case rang::control::nonBlocking:
            rang_implementation::isInputBlocking() = false;
            break;
        case rang::control::flush:
            rang_implementation::flushInput();
            break;
        case rang::control::lowLatency:
            rang_implementation::isInputLowLatency() = true;
            break;
        case rang::control::normalLatency:
            rang_implementation::isInputLowLatency() = false;
            break;
        default:
            // Ignore
            break;
    }
	return is;
}

}

#undef OS_LINUX
#undef OS_WIN
#undef OS_MAC

#endif /* ifndef RANG_DOT_HPP */

