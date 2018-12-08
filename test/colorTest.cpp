#include "rang.hpp"
#include <string>

using namespace std;
using namespace rang;

void printHeading(const string &heading)
{
    cout << '\n'
         << style::reset << heading << style::reset << bg::reset << fg::reset
         << endl;
}

template <typename CharT, typename Traits>
void test_colors(std::basic_ostream<CharT, Traits> &os, const winTerm opt)
{
    setWinTermMode(opt);

    printHeading("Text Style Test:");
    os << style::bold << " Bold " << style::reset;
    os << style::italic << " Italic " << style::reset;
    os << style::underline << " Underlined " << style::reset;
    os << style::dim << " Dim " << style::reset;
    os << style::conceal << " Conceal " << style::reset;
    os << style::reversed << " Reversed " << style::reset;
    os << style::blink << " Blink " << style::reset;
    os << style::rblink << " rBlink " << style::reset;
    os << style::crossed << " Crossed " << style::reset << endl;

    printHeading("Background Test:");
    os << bg::green << " Green " << bg::reset;
    os << bg::red << " Red " << bg::reset;
    os << bg::black << " Black " << bg::reset;
    os << bg::yellow << " Yellow " << bg::reset;
    os << bg::blue << " Blue " << bg::reset;
    os << bg::magenta << " Magenta " << bg::reset;
    os << bg::cyan << " Cyan " << bg::reset;
    os << bg::gray << " Grey " << bg::reset << endl;

    printHeading("Foreground Test:");
    os << fg::green << " Green " << fg::reset;
    os << fg::red << " Red " << fg::reset;
    os << fg::black << " Black " << fg::reset;
    os << fg::yellow << " Yellow " << fg::reset;
    os << fg::blue << " Blue " << fg::reset;
    os << fg::magenta << " Magenta " << fg::reset;
    os << fg::cyan << " Cyan " << fg::reset;
    os << fg::gray << " Grey " << fg::reset << endl;

    printHeading("Bright Background Test:");
    os << bgB::green << " Green " << bg::reset;
    os << bgB::red << " Red " << bg::reset;
    os << bgB::black << " Black " << bg::reset;
    os << bgB::yellow << " Yellow " << bg::reset;
    os << bgB::blue << " Blue " << bg::reset;
    os << bgB::magenta << " Magenta " << bg::reset;
    os << bgB::cyan << " Cyan " << bg::reset;
    os << bgB::gray << " Grey " << bg::reset << endl;

    printHeading("Bright Foreground Test:");
    os << fgB::green << " Green " << fg::reset;
    os << fgB::red << " Red " << fg::reset;
    os << fgB::black << " Black " << fg::reset;
    os << fgB::yellow << " Yellow " << fg::reset;
    os << fgB::blue << " Blue " << fg::reset;
    os << fgB::magenta << " Magenta " << fg::reset;
    os << fgB::cyan << " Cyan " << fg::reset;
    os << fgB::gray << " Grey " << fg::reset << endl;
}

void enumerateWinTerms()
{
    cout << endl;
    cout << "_________________________________________________________________";
    cout << "\n\n"
         << style::reset << style::bold << "Printing for WinTerm = Auto"
         << style::reset << bg::reset << fg::reset << '\n';
    cout << "_________________________________________________________________";
    test_colors(cout, winTerm::Auto);
    test_colors(clog, winTerm::Auto);
    test_colors(cerr, winTerm::Auto);
    cout << "-------------------------------------------------------------\n\n";

    cout << endl;
    cout << "_________________________________________________________________";
    cout << "\n\n"
         << style::reset << style::bold << "Printing for WinTerm = Ansi"
         << style::reset << bg::reset << fg::reset << '\n';
    cout << "_________________________________________________________________";
    test_colors(cout, winTerm::Ansi);
    test_colors(clog, winTerm::Ansi);
    test_colors(cerr, winTerm::Ansi);
    cout << "-------------------------------------------------------------\n\n";

    cout << endl;
    cout << "_________________________________________________________________";
    cout << "\n\n"
         << style::reset << style::bold << "Printing for WinTerm = Native"
         << style::reset << bg::reset << fg::reset << '\n';
    cout << "_________________________________________________________________";
    test_colors(cout, winTerm::Native);
    test_colors(clog, winTerm::Native);
    test_colors(cerr, winTerm::Native);
    cout << "-------------------------------------------------------------\n\n";
}

int main()
{
    cout << "\n\n\n"
         << style::reset << style::underline << style::bold << "Control = Auto"
         << style::reset << bg::reset << fg::reset << endl;
    setControlMode(control::Auto);
    enumerateWinTerms();

    cout << "\n\n\n"
         << style::reset << style::underline << style::bold
         << "Control = Force " << style::reset << bg::reset << fg::reset
         << endl;
    setControlMode(control::Force);
    enumerateWinTerms();

    cout << "\n\n\n"
         << style::reset << style::underline << style::bold << "Control = Off "
         << style::reset << bg::reset << fg::reset << endl;
    setControlMode(control::Off);
    enumerateWinTerms();
}
