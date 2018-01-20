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
        setControlMode(control::offColor);
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

        setControlMode(control::forceColor);
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

TEST_CASE("Rang printing to stdout/err")
{
    const string s = "Rang works with ";

    SUBCASE("output is to terminal")
    {
        setControlMode(control::forceColor);
        cout << fg::green << s << "cout" << style::reset << endl;
        clog << fg::blue << s << "clog" << style::reset << endl;
        cerr << fg::red << s << "cerr" << style::reset << endl;
        REQUIRE(1 == 1);
    }
}
