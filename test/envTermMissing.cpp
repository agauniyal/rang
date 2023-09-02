#include "../include/rang.hpp"
#include <cstdlib>

using std::cout;
using std::endl;
using std::getenv;

int main()
{
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    return 0;
#else
    const auto TERM = getenv("TERM");
    if (TERM != nullptr) {
        cout << "Unsetting $PATH: " << TERM << '\n';
        unsetenv("TERM");
    }
    cout << rang::fg::green << "===NO COLORS AS FALLBACK===" << endl;
    if (TERM != nullptr) {
        cout << "Setting $PATH: " << TERM << '\n';
        setenv("TERM", TERM, 1);
    }
#endif
}
