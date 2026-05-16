#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "rang.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace rang;

#if defined(__unix__) || defined(__unix) || defined(__linux__)
#define OS_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define OS_WIN
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MAC
#else
#error Unknown Platform
#endif


TEST_CASE("Rang printing with control::Off and cout")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    setControlMode(control::Off);

    SUBCASE("WinTerm = Native")
    {
        setWinTermMode(winTerm::Native);
        ofstream out(fileName);
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());
        cout << fg::blue << s << style::reset;
        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Auto")
    {
        setWinTermMode(winTerm::Auto);

        ofstream out(fileName);
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());
        cout << fg::blue << s << style::reset;
        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Ansi")
    {
        setWinTermMode(winTerm::Ansi);

        ofstream out(fileName);
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());
        cout << fg::blue << s << style::reset;
        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }
}

TEST_CASE("Rang printing with control::Force and cout")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    setControlMode(control::Force);

    SUBCASE("WinTerm = Native")
    {
        setWinTermMode(winTerm::Native);
        ofstream out(fileName);
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());
        cout << fg::blue << s << style::reset;
        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

#if defined(OS_LINUX) || defined(OS_MAC)
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }

    SUBCASE("WinTerm = Ansi")
    {
        setWinTermMode(winTerm::Ansi);

        ofstream out(fileName);
        streambuf *coutbuf = cout.rdbuf();
        cout.rdbuf(out.rdbuf());
        cout << fg::blue << s << style::reset;
        cout.rdbuf(coutbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
    }
}

TEST_CASE("Rang printing with control::Off and cerr")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    setControlMode(control::Off);

    SUBCASE("WinTerm = Native")
    {
        setWinTermMode(winTerm::Native);
        ofstream out(fileName);
        streambuf *cerrbuf = cerr.rdbuf();
        cerr.rdbuf(out.rdbuf());
        cerr << fg::blue << s << style::reset;
        cerr.rdbuf(cerrbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Auto")
    {
        setWinTermMode(winTerm::Auto);

        ofstream out(fileName);
        streambuf *cerrbuf = cerr.rdbuf();
        cerr.rdbuf(out.rdbuf());
        cerr << fg::blue << s << style::reset;
        cerr.rdbuf(cerrbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Ansi")
    {
        setWinTermMode(winTerm::Ansi);

        ofstream out(fileName);
        streambuf *cerrbuf = cerr.rdbuf();
        cerr.rdbuf(out.rdbuf());
        cerr << fg::blue << s << style::reset;
        cerr.rdbuf(cerrbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }
}

TEST_CASE("Rang printing with control::Force and cerr")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    setControlMode(control::Force);

    SUBCASE("WinTerm = Native")
    {
        setWinTermMode(winTerm::Native);
        ofstream out(fileName);
        streambuf *cerrbuf = cerr.rdbuf();
        cerr.rdbuf(out.rdbuf());
        cerr << fg::blue << s << style::reset;
        cerr.rdbuf(cerrbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

#if defined(OS_LINUX) || defined(OS_MAC)
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }

    SUBCASE("WinTerm = Ansi")
    {
        setWinTermMode(winTerm::Ansi);

        ofstream out(fileName);
        streambuf *cerrbuf = cerr.rdbuf();
        cerr.rdbuf(out.rdbuf());
        cerr << fg::blue << s << style::reset;
        cerr.rdbuf(cerrbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
    }
}

TEST_CASE("Rang printing with control::Off and clog")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    setControlMode(control::Off);

    SUBCASE("WinTerm = Native")
    {
        setWinTermMode(winTerm::Native);
        ofstream out(fileName);
        streambuf *clogbuf = clog.rdbuf();
        clog.rdbuf(out.rdbuf());
        clog << fg::blue << s << style::reset;
        clog.rdbuf(clogbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Auto")
    {
        setWinTermMode(winTerm::Auto);

        ofstream out(fileName);
        streambuf *clogbuf = clog.rdbuf();
        clog.rdbuf(out.rdbuf());
        clog << fg::blue << s << style::reset;
        clog.rdbuf(clogbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Ansi")
    {
        setWinTermMode(winTerm::Ansi);

        ofstream out(fileName);
        streambuf *clogbuf = clog.rdbuf();
        clog.rdbuf(out.rdbuf());
        clog << fg::blue << s << style::reset;
        clog.rdbuf(clogbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s == output);
    }
}

TEST_CASE("Rang printing with control::Force and clog")
{
    const string s        = "Hello World";
    const string fileName = "outoutoutout.txt";

    setControlMode(control::Force);

    SUBCASE("WinTerm = Native")
    {
        setWinTermMode(winTerm::Native);
        ofstream out(fileName);
        streambuf *clogbuf = clog.rdbuf();
        clog.rdbuf(out.rdbuf());
        clog << fg::blue << s << style::reset;
        clog.rdbuf(clogbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

#if defined(OS_LINUX) || defined(OS_MAC)
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }

    SUBCASE("WinTerm = Ansi")
    {
        setWinTermMode(winTerm::Ansi);

        ofstream out(fileName);
        streambuf *clogbuf = clog.rdbuf();
        clog.rdbuf(out.rdbuf());
        clog << fg::blue << s << style::reset;
        clog.rdbuf(clogbuf);
        out.close();

        ifstream in(fileName);
        string output;
        getline(in, output);

        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
    }
}
