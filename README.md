# rang 
[![Build Status](https://travis-ci.org/agauniyal/rang.svg?branch=master)](https://travis-ci.org/agauniyal/rang) 
[![Coverage Status](https://coveralls.io/repos/github/agauniyal/rang/badge.svg?branch=master)](https://coveralls.io/github/agauniyal/rang?branch=master)
[![Build status](https://ci.appveyor.com/api/projects/status/jqpdoelli38h2a7w?svg=true)](https://ci.appveyor.com/project/agauniyal/rang)

#####Colors for your Terminal.

![rang-demo](https://cloud.githubusercontent.com/assets/7630575/13501282/0bd00074-e18c-11e5-9848-5bd1f20566d9.gif)

------
##How to use it
#### No build system/package manager
1. You just need [rang.hpp](https://raw.githubusercontent.com/agauniyal/rang/master/include/rang.hpp) from include directory, use it as `#include "rang.hpp"`
2. Check out the [wiki](https://github.com/agauniyal/rang/wiki) ~~& maybe glance inside ***`test`*** directory to see some usage~~
3. ???
4. Profit!!

#### Using Meson build system
- Make a `rang.wrap` file inside your `subprojects/` directory with following content - 
```
[rang-git]
directory=rang
url=https://github.com/agauniyal/rang.git
revision=head
```
- Place this code inside your primary `meson.build` - `rang_dep = dependency('rang', fallback : ['rang', 'rang_dep'])`
- Make sure to include `rang_dep` as dependency wherever you need it.

------
##TODO
- [ ] support Windows versions < 10
