# rang [![Build Status](https://travis-ci.org/agauniyal/rang.svg?branch=master)](https://travis-ci.org/agauniyal/rang) [![Coverage Status](https://coveralls.io/repos/github/agauniyal/rang/badge.svg?branch=master)](https://coveralls.io/github/agauniyal/rang?branch=master)

#####Colors for your Terminal.

![rang-demo](https://cloud.githubusercontent.com/assets/7630575/13501282/0bd00074-e18c-11e5-9848-5bd1f20566d9.gif)

------
##How to use it
1. You just need *`rang.hpp`* from include directory, use it as `#include "rang.hpp"`
2. Check out the [wiki](https://github.com/agauniyal/rang/wiki) ~~& maybe glance inside ***`test`*** directory to see some usage~~
3. ???
4. Profit!!

###Package Managers/ Build systems
If you're using [Meson build system](https://github.com/mesonbuild/meson), it's as simple as dropping a `rang.wrap` file inside your `subprojects/` directory with following contents - 

```
[wrap-git]
directory=uWebSockets
url=https://github.com/uWebSockets/uWebSockets.git
revision=head
```
I'm accepting PRs for other package managers/build systems too.

------
##TODO
- [ ] support Windows versions < 10
