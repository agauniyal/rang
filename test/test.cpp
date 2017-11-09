#if defined(__MINGW32__) || defined(__MINGW64__)
#define _WIN32_WINNT 0x0600
#endif

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "rang.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace rang;

/*
The control::forceColor is used to write to terminal here in order to work
with appveyor terminal
*/

TEST_CASE("Rang printing to non-terminals")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    SUBCASE("OffColor")
    {
        ofstream out(fileName);

        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());

        // Make sure to turn color off
        cout << control::offColor;
        cout << fg::blue << s << style::reset;

        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("Force Color")
    {
        ofstream out(fileName);

        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());

        cout << control::forceColor;
        cout << fg::blue << s << style::reset;

        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

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
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }
}

TEST_CASE("Rang printing to stdout/err")
{
    const string s = "Rang works with ";

    SUBCASE("output is to terminal")
    {
        cout << control::forceColor << fg::green << s << "cout" << style::reset
             << endl;
        clog << fg::blue << s << "clog" << style::reset << endl;
        cerr << fg::red << s << "cerr" << style::reset << endl;
        REQUIRE(1 == 1);
    }
}
