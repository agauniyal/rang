#include "rang.hpp"

using namespace std;
using namespace rang;

int main()
{
    rang::setControlMode(control::forceColor);  // For appveyor terminal
    cout << endl
         << style::reset << bg::green << fg::gray
         << "If you're seeing green background, then rang works!"
         << style::reset << endl;
}
