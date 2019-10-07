#include "rang.hpp"
#include <string>
#include <chrono>
#include <thread>

using namespace std;

int main()
{
#if defined(_UNICODE) || defined(UNICODE)
    wcout << rang::cursor::setVisible(false);
    wcout << "Hidden for wcout!" << endl;
    this_thread::sleep_for(chrono::duration<int, ratio<1>>(2));
    wcout << rang::cursor::setVisible(true) << endl;
    wcout << "Shown for wcout!" << endl;
#else
    cout << rang::cursor::setVisible(false);
    cout << "Hidden for cout!" << endl;
    this_thread::sleep_for(chrono::duration<int, ratio<1>>(2));
    cout << rang::cursor::setVisible(true) << endl;
    cout << "Shown for cout!" << endl;
#endif
}
