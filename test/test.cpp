#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "rang.hpp"
#include <fstream>
#include <string>

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
    using namespace rang;
    const std::string s        = "Hello World";
    const std::string fileName = "outoutoutout.txt";

    rang::setControlMode(control::Off);

    SUBCASE("WinTerm = Native")
    {
        rang::setWinTermMode(winTerm::Native);
        std::ofstream out(fileName);
        std::streambuf *coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(out.rdbuf());
        std::cout << fg::blue << s << style::reset;
        std::cout.rdbuf(coutbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Auto")
    {
        rang::setWinTermMode(winTerm::Auto);

        std::ofstream out(fileName);
        std::streambuf *coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(out.rdbuf());
        std::cout << fg::blue << s << style::reset;
        std::cout.rdbuf(coutbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Ansi")
    {
        rang::setWinTermMode(winTerm::Ansi);

        std::ofstream out(fileName);
        std::streambuf *coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(out.rdbuf());
        std::cout << fg::blue << s << style::reset;
        std::cout.rdbuf(coutbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }
}

TEST_CASE("Rang printing with control::Force and cout")
{
    using namespace rang;
    const std::string s        = "Hello World";
    const std::string fileName = "outoutoutout.txt";

    rang::setControlMode(control::Force);

    SUBCASE("WinTerm = Native")
    {
        rang::setWinTermMode(winTerm::Native);
        std::ofstream out(fileName);
        std::streambuf *coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(out.rdbuf());
        std::cout << fg::blue << s << style::reset;
        std::cout.rdbuf(coutbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

#if defined(OS_LINUX) || defined(OS_MAC)
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }

    SUBCASE("WinTerm = Ansi")
    {
        rang::setWinTermMode(winTerm::Ansi);

        std::ofstream out(fileName);
        std::streambuf *coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(out.rdbuf());
        std::cout << fg::blue << s << style::reset;
        std::cout.rdbuf(coutbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
    }
}

TEST_CASE("Rang printing with control::Off and cerr")
{
    using namespace rang;
    const std::string s        = "Hello World";
    const std::string fileName = "outoutoutout.txt";

    rang::setControlMode(control::Off);

    SUBCASE("WinTerm = Native")
    {
        rang::setWinTermMode(winTerm::Native);
        std::ofstream out(fileName);
        std::streambuf *cerrbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out.rdbuf());
        std::cerr << fg::blue << s << style::reset;
        std::cerr.rdbuf(cerrbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Auto")
    {
        rang::setWinTermMode(winTerm::Auto);

        std::ofstream out(fileName);
        std::streambuf *cerrbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out.rdbuf());
        std::cerr << fg::blue << s << style::reset;
        std::cerr.rdbuf(cerrbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Ansi")
    {
        rang::setWinTermMode(winTerm::Ansi);

        std::ofstream out(fileName);
        std::streambuf *cerrbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out.rdbuf());
        std::cerr << fg::blue << s << style::reset;
        std::cerr.rdbuf(cerrbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }
}

TEST_CASE("Rang printing with control::Force and cerr")
{
    using namespace rang;
    const std::string s        = "Hello World";
    const std::string fileName = "outoutoutout.txt";

    rang::setControlMode(control::Force);

    SUBCASE("WinTerm = Native")
    {
        rang::setWinTermMode(winTerm::Native);
        std::ofstream out(fileName);
        std::streambuf *cerrbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out.rdbuf());
        std::cerr << fg::blue << s << style::reset;
        std::cerr.rdbuf(cerrbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

#if defined(OS_LINUX) || defined(OS_MAC)
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }

    SUBCASE("WinTerm = Ansi")
    {
        rang::setWinTermMode(winTerm::Ansi);

        std::ofstream out(fileName);
        std::streambuf *cerrbuf = std::cerr.rdbuf();
        std::cerr.rdbuf(out.rdbuf());
        std::cerr << fg::blue << s << style::reset;
        std::cerr.rdbuf(cerrbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
    }
}

TEST_CASE("Rang printing with control::Off and clog")
{
    using namespace rang;
    const std::string s        = "Hello World";
    const std::string fileName = "outoutoutout.txt";

    rang::setControlMode(control::Off);

    SUBCASE("WinTerm = Native")
    {
        rang::setWinTermMode(winTerm::Native);
        std::ofstream out(fileName);
        std::streambuf *clogbuf = std::clog.rdbuf();
        std::clog.rdbuf(out.rdbuf());
        std::clog << fg::blue << s << style::reset;
        std::clog.rdbuf(clogbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Auto")
    {
        rang::setWinTermMode(winTerm::Auto);

        std::ofstream out(fileName);
        std::streambuf *clogbuf = std::clog.rdbuf();
        std::clog.rdbuf(out.rdbuf());
        std::clog << fg::blue << s << style::reset;
        std::clog.rdbuf(clogbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }

    SUBCASE("WinTerm = Ansi")
    {
        rang::setWinTermMode(winTerm::Ansi);

        std::ofstream out(fileName);
        std::streambuf *clogbuf = std::clog.rdbuf();
        std::clog.rdbuf(out.rdbuf());
        std::clog << fg::blue << s << style::reset;
        std::clog.rdbuf(clogbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s == output);
    }
}

TEST_CASE("Rang printing with control::Force and clog")
{
    using namespace rang;
    const std::string s        = "Hello World";
    const std::string fileName = "outoutoutout.txt";

    rang::setControlMode(control::Force);

    SUBCASE("WinTerm = Native")
    {
        rang::setWinTermMode(winTerm::Native);
        std::ofstream out(fileName);
        std::streambuf *clogbuf = std::clog.rdbuf();
        std::clog.rdbuf(out.rdbuf());
        std::clog << fg::blue << s << style::reset;
        std::clog.rdbuf(clogbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

#if defined(OS_LINUX) || defined(OS_MAC)
        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
#elif defined(OS_WIN)
        REQUIRE(s == output);
#endif
    }

    SUBCASE("WinTerm = Ansi")
    {
        rang::setWinTermMode(winTerm::Ansi);

        std::ofstream out(fileName);
        std::streambuf *clogbuf = std::clog.rdbuf();
        std::clog.rdbuf(out.rdbuf());
        std::clog << fg::blue << s << style::reset;
        std::clog.rdbuf(clogbuf);
        out.close();

        std::ifstream in(fileName);
        std::string output;
        std::getline(in, output);

        REQUIRE(s != output);
        REQUIRE(s.size() < output.size());
    }
}
