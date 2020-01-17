# libukui

## Overview

`libukui` represents the core library of UKUi providing essential functionality
needed by nearly all of its components.

## Installation

### Sources

Its runtime dependencies are libxss, KWindowSystem, qtbase, qtx11extras and
[libqtxdg](https://github.com/ukui/libqtxdg).
Additional build dependencies are CMake and optionally Git to pull latest VCS
checkouts. The localization files were outsourced to repository
[ukui-l10n](https://github.com/ukui/ukui-l10n) so the corresponding dependencies
are needed, too. Please refer to this repository's `README.md` for further
information.

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX`
will normally have to be set to `/usr`, depending on the way library paths are
dealt with on 64bit systems variables like `CMAKE_INSTALL_LIBDIR` may have to be
set as well.

To build run `make`, to install `make install` which accepts variable `DESTDIR`
as usual.

### Binary packages

The library is provided by all major Linux distributions like Arch Linux, Debian,
Fedora and openSUSE. Just use your package manager to search for string `libukui`.


### Translation (Weblate)

<a href="https://weblate.ukui.org/projects/ukui/libukui/">
<img src="https://weblate.ukui.org/widgets/ukui/-/libukui/multi-auto.svg" alt="Translation status" />
</a>
