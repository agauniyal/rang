# rang [![wercker status](https://app.wercker.com/status/58b65d3b469ae164da8c741c81a2d7fd/s/master "wercker status")](https://app.wercker.com/project/byKey/58b65d3b469ae164da8c741c81a2d7fd) [![Build status](https://ci.appveyor.com/api/projects/status/jqpdoelli38h2a7w?svg=true)](https://ci.appveyor.com/project/agauniyal/rang) [![codecov](https://codecov.io/gh/agauniyal/rang/branch/master/graph/badge.svg)](https://codecov.io/gh/agauniyal/rang) [ ![Download](https://api.bintray.com/packages/agauniyal/rang/rang%3Arang/images/download.svg) ](https://bintray.com/agauniyal/rang/rang%3Arang/_latestVersion)  [![Open Source Helpers](https://www.codetriage.com/agauniyal/rang/badges/users.svg)](https://www.codetriage.com/agauniyal/rang)

[![wercker status](https://app.wercker.com/status/58b65d3b469ae164da8c741c81a2d7fd/m/ "wercker status")](https://app.wercker.com/project/byKey/58b65d3b469ae164da8c741c81a2d7fd)

##### Colors for your Terminal.

![rang-demo](https://cloud.githubusercontent.com/assets/7630575/13501282/0bd00074-e18c-11e5-9848-5bd1f20566d9.gif)
<details>
 <summary>Windows Demo</summary>

![rang-windows-demo](https://cloud.githubusercontent.com/assets/11349690/19836886/8134975e-9ebe-11e6-9ee4-c4657784ff3b.gif)
</details>


Example usage
-------------

```c++
#include "rang.hpp"

using namespace std;
using namespace rang;

int main()
{
    cout << "Plain old text"
         << style::bold << "Rang styled text!!"
         << style::reset << endl;
}
```

Dependencies
------------
*rang* only depends on [C++ standard library](http://en.cppreference.com/w/cpp/header), `unistd.h` system header on unix and `windows.h` & `io.h` system headers on windows based systems. In other words, you don't need any 3rd party dependencies.


Installation
------------

*rang* is a single header-only library. Put `rang.hpp` in the [include](include) folder directly into the project source tree or somewhere reachable from your project.

Or, if you use the [conan package manager](https://www.conan.io/), follow these steps:

1. Add a reference to *rang* to the *requires* section of your project's `conanfile.txt` file:

        [requires]
        rang/3.1.0@rang/stable

2. Run conan's install command:

        conan install


## How to use

*Rang* uses iostream objects - `cout`/`clog`/`cerr` to apply attributes to output text. Since *rang* aims to support both windows and unix like systems, it takes care of the os specific details and tries to provide a uniform interface. Due to incompatiblities b/w different OS versions, not all kinds of attributes are supported on every system so rang will try to skip the ones which might produce garbage(instead of pushing random ANSI escape codes on your streams). Detection of tty is also handled internally so you don't need to check if application user might redirect output to a file.

> **Need support for non-ansi terminals? Check out [Termdb](https://github.com/agauniyal/termdb) which supports virtually all terminals and their capablities.**

Apart from setting text attributes, you can also ask rang to override its default behaviour through these methods -
```cpp
void rang::setControlMode(rang::control);
```
where `rang::control` takes
 - `control::Auto` - Automatically detects whether terminal supports color or not(**Default**)
 - `control::Off` - Turn off colors completely
 - `control::Force` - Force colors even if terminal doesn't supports them or output is redirected to non-terminal

```cpp
void rang::setWinTermMode(rang::winTerm);
```
where `rang::winTerm` takes
 - `winTerm::Auto` - Checks for newer windows and picks Ansi otherwise falls back to Native(**Default**)
 - `winTerm::Native` - This method is supported in all versions of windows but supports less attributes
 - `winTerm::Ansi` - This method is supported in newer versions of windows and supports rich variety of attributes


Supported attributes with their compatiblity are listed below -

**Text Styles**:

| Code | Linux/Win/Others | Old Win
| ---- | --------- | ------ |
| `rang::style::bold`      | yes   | yes   |
| `rang::style::dim`       | yes   | no    |
| `rang::style::italic`    | yes   | no    |
| `rang::style::underline` | yes   | no    |
| `rang::style::blink`     | no    | no    |
| `rang::style::rblink`    | no    | no    |
| `rang::style::reversed`  | yes   | yes   |
| `rang::style::conceal`   | maybe | yes   |
| `rang::style::crossed`   | yes   | no    |

**Text Color**:

| Code | Linux/Win/Others | Old Win
| ---- | --------- | ------ |
| `rang::fg::black`     | yes | yes |
| `rang::fg::red`       | yes | yes |
| `rang::fg::green`     | yes | yes |
| `rang::fg::yellow`    | yes | yes |
| `rang::fg::blue`      | yes | yes |
| `rang::fg::magenta`   | yes | yes |
| `rang::fg::cyan`      | yes | yes |
| `rang::fg::gray`      | yes | yes |

**Background Color**:

| Code | Linux/Win/Others | Old Win
| ---- | --------- | ------ |
| `rang::bg::black`     | yes | yes |
| `rang::bg::red`       | yes | yes |
| `rang::bg::green`     | yes | yes |
| `rang::bg::yellow`    | yes | yes |
| `rang::bg::blue`      | yes | yes |
| `rang::bg::magenta`   | yes | yes |
| `rang::bg::cyan`      | yes | yes |
| `rang::bg::gray`      | yes | yes |

**Bright Foreground Color**:

| Code | Linux/Win/Others | Old Win
| ---- | --------- | ------ |
| `rang::fgB::black`     | yes | yes |
| `rang::fgB::red`       | yes | yes |
| `rang::fgB::green`     | yes | yes |
| `rang::fgB::yellow`    | yes | yes |
| `rang::fgB::blue`      | yes | yes |
| `rang::fgB::magenta`   | yes | yes |
| `rang::fgB::cyan`      | yes | yes |
| `rang::fgB::gray`      | yes | yes |

**Bright Background Color**:

| Code | Linux/Win/Others | Old Win
| ---- | --------- | ------ |
| `rang::bgB::black`     | yes | yes |
| `rang::bgB::red`       | yes | yes |
| `rang::bgB::green`     | yes | yes |
| `rang::bgB::yellow`    | yes | yes |
| `rang::bgB::blue`      | yes | yes |
| `rang::bgB::magenta`   | yes | yes |
| `rang::bgB::cyan`      | yes | yes |
| `rang::bgB::gray`      | yes | yes |

**Reset Styles/Colors**:

| Code | Linux/Win/Others | Old Win
| ---- | --------- | ------ |
| `rang::style::reset`  | yes   | yes |
| `rang::fg::reset`     | yes   | yes |
| `rang::bg::reset`     | yes   | yes |

-----
## My terminal is not detected/gets garbage output!

Check your env variable `TERM`'s value. Then open an issue [here](https://github.com/agauniyal/rang/issues/new) and make sure to mention `TERM`'s value along with your terminal name.

## Redirecting `cout`/`cerr`/`clog` rdbuf?

Rang doesn't interfere if you try to redirect `cout`/`cerr`/`clog` to somewhere else and leaves the decision to the library user. Make sure you've read this [conversation](https://github.com/agauniyal/rang/pull/77#issuecomment-360991652) and check out the example code [here](https://gist.github.com/kingseva/a918ec66079a9475f19642ec31276a21).
